#ifndef COSMOPOLITAN_THIRD_PARTY_SMALLZ4_SMALLZ4_H_
#define COSMOPOLITAN_THIRD_PARTY_SMALLZ4_SMALLZ4_H_
#include "libc/serialize.h"
#include "third_party/libcxx/vector"

/**
 * LZ4 compression with optimal parsing
 *
 * See smallz4.cc for a basic I/O interface you can easily replace it by
 * a in-memory version then all you have to do is:
 *
 *     smallz4::lz4(GET_BYTES, SEND_BYTES);
 *
 * For more advanced stuff, you can call lz4 with up to four parameters
 * (incl. max chain length and a dictionary)
 */
class smallz4 {
 public:
  // read  several bytes, see getBytesFromIn() in smallz4.cpp for a basic
  // implementation
  typedef size_t (*GET_BYTES)(void* data, size_t numBytes, void* userPtr);
  // write several bytes, see sendBytesToOut() in smallz4.cpp for a basic
  // implementation
  typedef void (*SEND_BYTES)(const void* data, size_t numBytes, void* userPtr);

  /// compress everything in input stream (accessed via getByte) and write to
  /// output stream (via send)
  static void lz4(GET_BYTES getBytes, SEND_BYTES sendBytes,
                  unsigned short maxChainLength = MaxChainLength,
                  bool useLegacyFormat = false, void* userPtr = NULL) {
    lz4(getBytes, sendBytes, maxChainLength, std::vector<unsigned char>(),
        useLegacyFormat, userPtr);
  }

  /// compress everything in input stream (accessed via getByte) and write to
  /// output stream (via send)
  static void lz4(
      GET_BYTES getBytes, SEND_BYTES sendBytes, unsigned short maxChainLength,
      const std::vector<unsigned char>& dictionary,  // predefined dictionary
      bool useLegacyFormat =
          false,  // old format is 7 bytes smaller if input < 8 MB
      void* userPtr = NULL) {
    smallz4 obj(maxChainLength);
    obj.compress(getBytes, sendBytes, dictionary, useLegacyFormat, userPtr);
  }

  /// version string
  static const char* const getVersion() {
    return "1.5";
  }

  // compression level thresholds, made public because I display them in the
  // help screen ...
  enum {
    /// greedy mode for short chains (compression level <= 3) instead of optimal
    /// parsing / lazy evaluation
    ShortChainsGreedy = 3,
    /// lazy evaluation for medium-sized chains (compression level > 3 and <= 6)
    ShortChainsLazy = 6
  };

  // ----- END OF PUBLIC INTERFACE -----
 private:
  // ----- constants and types -----

  /// a block can be up to 4 MB, so uint32_t would suffice but uint64_t is quite
  /// a bit faster on my x64 machine
  typedef uint64_t Length;
  /// matches must start within the most recent 64k
  typedef uint16_t Distance;

  enum {
    /// each match's length must be >= 4
    MinMatch = 4,
    /// a literal needs one byte
    JustLiteral = 1,
    /// last match must not be closer than 12 bytes to the end
    BlockEndNoMatch = 12,
    /// last 5 bytes must be literals, no matching allowed
    BlockEndLiterals = 5,

    /// match finder's hash table size (2^HashBits entries, must be less than
    /// 32)
    HashBits = 20,
    HashSize = 1 << HashBits,

    /// input buffer size, can be any number but zero ;-)
    BufferSize = 1024,

    /// maximum match distance, must be power of 2 minus 1
    MaxDistance = 65535,
    /// marker for "no match"
    EndOfChain = 0,
    /// stop match finding after MaxChainLength steps (default is unlimited =>
    /// optimal parsing)
    MaxChainLength = MaxDistance,

    /// significantly speed up parsing if the same byte is repeated a lot, may
    /// cause sub-optimal compression
    MaxSameLetter = 19 + 255 * 256,  // was: 19 + 255,

    /// maximum block size as defined in LZ4 spec: {
    /// 0,0,0,0,64*1024,256*1024,1024*1024,4*1024*1024 } I only work with the
    /// biggest maximum block size (7)
    //  note: xxhash header checksum is precalculated only for 7, too
    MaxBlockSizeId = 7,
    MaxBlockSize = 4 * 1024 * 1024,

    /// legacy format has a fixed block size of 8 MB
    MaxBlockSizeLegacy = 8 * 1024 * 1024,

    /// number of literals and match length is encoded in several bytes, max.
    /// 255 per byte
    MaxLengthCode = 255
  };

  //  ----- one and only variable ... -----

  /// how many matches are checked in findLongestMatch, lower values yield
  /// faster encoding at the cost of worse compression ratio
  unsigned short maxChainLength;

  //  ----- code -----

  /// match
  struct Match {
    /// length of match
    Length length;
    /// start of match
    Distance distance;
  };

  /// create new compressor (only invoked by lz4)
  explicit smallz4(unsigned short newMaxChainLength = MaxChainLength)
      : maxChainLength(newMaxChainLength)  // => no limit, but can be changed by
                                           // setMaxChainLength
  {
  }

  /// return true, if the four bytes at *a and *b match
  inline static bool match4(const void* const a, const void* const b) {
    return READ32LE(a) == READ32LE(b);
  }

  /// simple hash function, input: 32 bits, output: HashBits bits (by default:
  /// 20)
  inline static uint32_t getHash32(uint32_t fourBytes) {
    // taken from https://en.wikipedia.org/wiki/Linear_congruential_generator
    const uint32_t HashMultiplier = 48271;
    return ((fourBytes * HashMultiplier) >> (32 - HashBits)) & (HashSize - 1);
  }

  /// find longest match of data[pos] between data[begin] and data[end], use
  /// match chain
  Match findLongestMatch(const unsigned char* const data, uint64_t pos,
                         uint64_t begin, uint64_t end,
                         const Distance* const chain) const {
    Match result;
    result.length = JustLiteral;  // assume a literal => one byte

    // compression level: look only at the first n entries of the match chain
    unsigned short stepsLeft = maxChainLength;
    // findLongestMatch() shouldn't be called when maxChainLength = 0
    // (uncompressed)

    // pointer to position that is currently analyzed (which we try to find a
    // great match for)
    const unsigned char* const current = data + pos - begin;
    // don't match beyond this point
    const unsigned char* const stop = current + end - pos;

    // get distance to previous match, abort if 0 => not existing
    Distance distance = chain[pos & MaxDistance];
    int64_t totalDistance = 0;
    while (distance != EndOfChain) {
      // chain goes too far back ?
      totalDistance += distance;
      if (totalDistance > MaxDistance) break;  // can't match beyond 64k

      // prepare next position
      distance = chain[(pos - totalDistance) & MaxDistance];

      // let's introduce a new pointer atLeast that points to the first "new"
      // byte of a potential longer match
      const unsigned char* const atLeast = current + result.length + 1;
      // impossible to find a longer match because not enough bytes left ?
      if (atLeast > stop) break;

      // the idea is to split the comparison algorithm into 2 phases
      // (1) scan backward from atLeast to current, abort if mismatch
      // (2) scan forward  until a mismatch is found and store length/distance
      // of this new best match current                  atLeast
      //    |                        |
      //    -<<<<<<<< phase 1 <<<<<<<<
      //                              >>> phase 2 >>>
      // main reason for phase 1:
      // - both byte sequences start with the same bytes, quite likely they are
      // very similar
      // - there is a good chance that if they differ, then their last bytes
      // differ
      // => checking the last first increases the probability that a mismatch is
      // detected as early as possible

      // compare 4 bytes at once
      const Length CheckAtOnce = 4;

      // all bytes between current and atLeast shall be identical
      const unsigned char* phase1 =
          atLeast - CheckAtOnce;  // minus 4 because match4 checks 4 bytes
      while (phase1 > current && match4(phase1, phase1 - totalDistance))
        phase1 -= CheckAtOnce;
      // note: - the first four bytes always match
      //       - in the last iteration, phase1 points either at current + 1 or
      //       current + 2 or current + 3
      //       - therefore we compare a few bytes twice => but a check to skip
      //       these checks is more expensive

      // mismatch ? (the while-loop was aborted)
      if (phase1 > current) continue;

      // we have a new best match, now scan forward
      const unsigned char* phase2 = atLeast;

      // fast loop: check four bytes at once
      while (phase2 + CheckAtOnce <= stop &&
             match4(phase2, phase2 - totalDistance))
        phase2 += CheckAtOnce;
      // slow loop: check the last 1/2/3 bytes
      while (phase2 < stop && *phase2 == *(phase2 - totalDistance)) phase2++;

      // store new best match
      result.distance = Distance(totalDistance);
      result.length = Length(phase2 - current);

      // stop searching on lower compression levels
      if (--stepsLeft == 0) break;
    }

    return result;
  }

  /// create shortest output
  /** data points to block's begin; we need it to extract literals **/
  static std::vector<unsigned char> selectBestMatches(
      const std::vector<Match>& matches, const unsigned char* const data) {
    // store encoded data
    std::vector<unsigned char> result;
    result.reserve(matches.size());

    // indices of current run of literals
    size_t literalsFrom = 0;
    size_t numLiterals = 0;

    bool lastToken = false;

    // walk through the whole block
    for (size_t offset = 0;
         offset < matches.size();)  // increment inside of loop
    {
      // get best cost-weighted match
      Match match = matches[offset];

      // if no match, then count literals instead
      if (match.length <= JustLiteral) {
        // first literal ? need to reset pointers of current sequence of
        // literals
        if (numLiterals == 0) literalsFrom = offset;

        // add one more literal to current sequence
        numLiterals++;

        // next match
        offset++;

        // continue unless it's the last literal
        if (offset < matches.size()) continue;

        lastToken = true;
      } else {
        // skip unused matches
        offset += match.length;
      }

      // store match length (4 is implied because it's the minimum match length)
      int matchLength = int(match.length) - MinMatch;

      // last token has zero length
      if (lastToken) matchLength = 0;

      // token consists of match length and number of literals, let's start with
      // match length ...
      unsigned char token =
          (matchLength < 15) ? (unsigned char)matchLength : 15;

      // >= 15 literals ? (extra bytes to store length)
      if (numLiterals < 15) {
        // add number of literals in higher four bits
        token |= numLiterals << 4;
        result.push_back(token);
      } else {
        // set all higher four bits, the following bytes with determine the
        // exact number of literals
        result.push_back(token | 0xF0);

        // 15 is already encoded in token
        int encodeNumLiterals = int(numLiterals) - 15;

        // emit 255 until remainder is below 255
        while (encodeNumLiterals >= MaxLengthCode) {
          result.push_back(MaxLengthCode);
          encodeNumLiterals -= MaxLengthCode;
        }
        // and the last byte (can be zero, too)
        result.push_back((unsigned char)encodeNumLiterals);
      }
      // copy literals
      if (numLiterals > 0) {
        result.insert(result.end(), data + literalsFrom,
                      data + literalsFrom + numLiterals);

        // last token doesn't have a match
        if (lastToken) break;

        // reset
        numLiterals = 0;
      }

      // distance stored in 16 bits / little endian
      result.push_back(match.distance & 0xFF);
      result.push_back(match.distance >> 8);

      // >= 15+4 bytes matched
      if (matchLength >= 15) {
        // 15 is already encoded in token
        matchLength -= 15;
        // emit 255 until remainder is below 255
        while (matchLength >= MaxLengthCode) {
          result.push_back(MaxLengthCode);
          matchLength -= MaxLengthCode;
        }
        // and the last byte (can be zero, too)
        result.push_back((unsigned char)matchLength);
      }
    }

    return result;
  }

  /// walk backwards through all matches and compute number of compressed bytes
  /// from current position to the end of the block
  /** note: matches are modified (shortened length) if necessary **/
  static void estimateCosts(std::vector<Match>& matches) {
    const size_t blockEnd = matches.size();

    // equals the number of bytes after compression
    typedef uint32_t Cost;
    // minimum cost from this position to the end of the current block
    std::vector<Cost> cost(matches.size(), 0);
    // "cost" represents the number of bytes needed

    // the last bytes must always be literals
    Length numLiterals = BlockEndLiterals;
    // backwards optimal parsing
    for (int64_t i = (int64_t)blockEnd - (1 + BlockEndLiterals); i >= 0;
         i--)  // ignore the last 5 bytes, they are always literals
    {
      // if encoded as a literal
      numLiterals++;
      Length bestLength = JustLiteral;
      // such a literal "costs" 1 byte
      Cost minCost = cost[i + 1] + JustLiteral;

      // an extra length byte is required for every 255 literals
      if (numLiterals >= 15) {
        // same as: if ((numLiterals - 15) % MaxLengthCode == 0)
        // but I try hard to avoid the slow modulo function
        if (numLiterals == 15 || (numLiterals >= 15 + MaxLengthCode &&
                                  (numLiterals - 15) % MaxLengthCode == 0))
          minCost++;
      }

      // let's look at the longest match, almost always more efficient that the
      // plain literals
      Match match = matches[i];

      // very long self-referencing matches can slow down the program A LOT
      if (match.length >= MaxSameLetter && match.distance == 1) {
        // assume that longest match is always the best match
        // NOTE: this assumption might not be optimal !
        bestLength = match.length;
        minCost =
            cost[i + match.length] + 1 + 2 + 1 + Cost(match.length - 19) / 255;
      } else {
        // this is the core optimization loop

        // overhead of encoding a match: token (1 byte) + offset (2 bytes) +
        // sometimes extra bytes for long matches
        Cost extraCost = 1 + 2;
        Length nextCostIncrease = 18;  // need one more byte for 19+ long
                                       // matches (next increase: 19+255*x)

        // try all match lengths (start with short ones)
        for (Length length = MinMatch; length <= match.length; length++) {
          // token (1 byte) + offset (2 bytes) + extra bytes for long matches
          Cost currentCost = cost[i + length] + extraCost;
          // better choice ?
          if (currentCost <= minCost) {
            // regarding the if-condition:
            // "<"  prefers literals and shorter matches
            // "<=" prefers longer matches
            // they should produce the same number of bytes (because of the same
            // cost)
            // ... but every now and then it doesn't !
            // that's why: too many consecutive literals require an extra length
            // byte (which we took into consideration a few lines above) but we
            // only looked at literals beyond the current position if there are
            // many literal in front of the current position then it may be
            // better to emit a match with the same cost as the literals at the
            // current position
            // => it "breaks" the long chain of literals and removes the extra
            // length byte
            minCost = currentCost;
            bestLength = length;
            // performance-wise, a long match is usually faster during decoding
            // than multiple short matches on the other hand, literals are
            // faster than short matches as well (assuming same cost)
          }

          // very long matches need extra bytes for encoding match length
          if (length == nextCostIncrease) {
            extraCost++;
            nextCostIncrease += MaxLengthCode;
          }
        }
      }

      // store lowest cost so far
      cost[i] = minCost;

      // and adjust best match
      matches[i].length = bestLength;

      // reset number of literals if a match was chosen
      if (bestLength != JustLiteral) numLiterals = 0;

      // note: if bestLength is smaller than the previous matches[i].length then
      // there might be a closer match
      //       which could be more cache-friendly (=> faster decoding)
    }
  }

  /// compress everything in input stream (accessed via getByte) and write to
  /// output stream (via send), improve compression with a predefined dictionary
  void compress(GET_BYTES getBytes, SEND_BYTES sendBytes,
                const std::vector<unsigned char>& dictionary,
                bool useLegacyFormat, void* userPtr) const {
    // ==================== write header ====================
    if (useLegacyFormat) {
      // magic bytes
      const unsigned char header[] = {0x02, 0x21, 0x4C, 0x18};
      sendBytes(header, sizeof(header), userPtr);
    } else {
      // frame header
      const unsigned char header[] = {
          0x04, 0x22, 0x4D,
          0x18,    // magic bytes
          1 << 6,  // flags: no checksums, blocks depend on each other and no
                   // dictionary ID
          MaxBlockSizeId << 4,  // max blocksize
          0xDF                  // header checksum (precomputed)
      };
      sendBytes(header, sizeof(header), userPtr);
    }

    // ==================== declarations ====================
    // change read buffer size as you like
    unsigned char buffer[BufferSize];

    // read the file in chunks/blocks, data will contain only bytes which are
    // relevant for the current block
    std::vector<unsigned char> data;

    // file position corresponding to data[0]
    size_t dataZero = 0;
    // last already read position
    size_t numRead = 0;

    // passthru data ? (but still wrap it in LZ4 format)
    const bool uncompressed = (maxChainLength == 0);

    // last time we saw a hash
    const uint64_t NoLastHash = ~0;  // = -1
    std::vector<uint64_t> lastHash(HashSize, NoLastHash);

    // previous position which starts with the same bytes
    std::vector<Distance> previousHash(
        MaxDistance + 1,
        Distance(EndOfChain));  // long chains based on my simple hash
    std::vector<Distance> previousExact(
        MaxDistance + 1,
        Distance(EndOfChain));  // shorter chains based on exact matching of the
                                // first four bytes
    // these two containers are essential for match finding:
    // 1. I compute a hash of four byte
    // 2. in lastHash is the location of the most recent block of four byte with
    // that same hash
    // 3. due to hash collisions, several groups of four bytes may yield the
    // same hash
    // 4. so for each location I can look up the previous location of the same
    // hash in previousHash
    // 5. basically it's a chain of memory locations where potential matches
    // start
    // 5. I follow this hash chain until I find exactly the same four bytes I
    // was looking for
    // 6. then I switch to a sparser chain: previousExact
    // 7. it's basically the same idea as previousHash but this time not the
    // hash but the first four bytes must be identical
    // 8. previousExact will be used by findLongestMatch: it compare all such
    // strings a figures out which is the longest match

    // And why do I have to do it in such a complicated way ?
    // - well, there are 2^32 combinations of four bytes
    // - so that there are 2^32 potential chains
    // - most combinations just don't occur and occupy no space but I still have
    // to keep their "entry point" (which are empty/invalid)
    // - that would be at least 16 GBytes RAM (2^32 x 4 bytes)
    // - my hashing algorithm reduces the 2^32 combinations to 2^20 hashes (see
    // hashBits), that's about 8 MBytes RAM
    // - thus only 2^20 entry points and at most 2^20 hash chains which is
    // easily manageable
    // ... in the end it's all about conserving memory !
    // (total memory consumption of smallz4 is about 64 MBytes)

    // first and last offset of a block (nextBlock is end-of-block plus 1)
    uint64_t lastBlock = 0;
    uint64_t nextBlock = 0;
    bool parseDictionary = !dictionary.empty();

    // main loop, processes one block per iteration
    while (true) {
      // ==================== start new block ====================
      // first byte of the currently processed block (std::vector data may
      // contain the last 64k of the previous block, too)
      const unsigned char* dataBlock = NULL;

      // prepend dictionary
      if (parseDictionary) {
        // resize dictionary to 64k (minus 1 because we can only match the last
        // 65535 bytes of the dictionary => MaxDistance)
        if (dictionary.size() < MaxDistance) {
          // dictionary is smaller than 64k, prepend garbage data
          size_t unused = MaxDistance - dictionary.size();
          data.resize(unused, 0);
          data.insert(data.end(), dictionary.begin(), dictionary.end());
        } else
          // copy only the most recent 64k of the dictionary
          data.insert(data.end(),
                      dictionary.begin() + dictionary.size() - MaxDistance,
                      dictionary.end());

        nextBlock = data.size();
        numRead = data.size();
      }

      // read more bytes from input
      size_t maxBlockSize = useLegacyFormat ? MaxBlockSizeLegacy : MaxBlockSize;
      while (numRead - nextBlock < maxBlockSize) {
        // buffer can be significantly smaller than MaxBlockSize, that's the
        // only reason for this while-block
        size_t incoming = getBytes(buffer, BufferSize, userPtr);
        // no more data ?
        if (incoming == 0) break;

        // add bytes to buffer
        numRead += incoming;
        data.insert(data.end(), buffer, buffer + incoming);
      }

      // no more data ? => WE'RE DONE !
      if (nextBlock == numRead) break;

      // determine block borders
      lastBlock = nextBlock;
      nextBlock += maxBlockSize;
      // not beyond end-of-file
      if (nextBlock > numRead) nextBlock = numRead;

      // pointer to first byte of the currently processed block (the std::vector
      // container named data may contain the last 64k of the previous block,
      // too)
      dataBlock = &data[lastBlock - dataZero];

      const uint64_t blockSize = nextBlock - lastBlock;

      // ==================== full match finder ====================

      // greedy mode is much faster but produces larger output
      const bool isGreedy = (maxChainLength <= ShortChainsGreedy);
      // lazy evaluation: if there is a match, then try running match finder on
      // next position, too, but not after that
      const bool isLazy = !isGreedy && (maxChainLength <= ShortChainsLazy);
      // skip match finding on the next x bytes in greedy mode
      Length skipMatches = 0;
      // allow match finding on the next byte but skip afterwards (in lazy mode)
      bool lazyEvaluation = false;

      // the last literals of the previous block skipped matching, so they are
      // missing from the hash chains
      int64_t lookback = int64_t(dataZero);
      if (lookback > BlockEndNoMatch && !parseDictionary)
        lookback = BlockEndNoMatch;
      if (parseDictionary) lookback = int64_t(dictionary.size());
      // so let's go back a few bytes
      lookback = -lookback;
      // ... but not in legacy mode
      if (useLegacyFormat || uncompressed) lookback = 0;

      std::vector<Match> matches(uncompressed ? 0 : blockSize);
      // find longest matches for each position (skip if level=0 which means
      // "uncompressed")
      int64_t i;
      for (i = lookback;
           i + BlockEndNoMatch <= int64_t(blockSize) && !uncompressed; i++) {
        // detect self-matching
        if (i > 0 && dataBlock[i] == dataBlock[i - 1]) {
          Match prevMatch = matches[i - 1];
          // predecessor had the same match ?
          if (prevMatch.distance == 1 &&
              prevMatch.length > MaxSameLetter)  // TODO: handle very long
                                                 // self-referencing matches
          {
            // just copy predecessor without further (expensive) optimizations
            matches[i].distance = 1;
            matches[i].length = prevMatch.length - 1;
            continue;
          }
        }

        // read next four bytes
        const uint32_t four = READ32LE(dataBlock + i);
        // convert to a shorter hash
        const uint32_t hash = getHash32(four);

        // get most recent position of this hash
        uint64_t lastHashMatch = lastHash[hash];
        // and store current position
        lastHash[hash] = i + lastBlock;

        // remember: i could be negative, too
        Distance prevIndex =
            (i + MaxDistance + 1) &
            MaxDistance;  // actually the same as i & MaxDistance

        // no predecessor / no hash chain available ?
        if (lastHashMatch == NoLastHash) {
          previousHash[prevIndex] = EndOfChain;
          previousExact[prevIndex] = EndOfChain;
          continue;
        }

        // most recent hash match too far away ?
        uint64_t distance = lastHash[hash] - lastHashMatch;
        if (distance > MaxDistance) {
          previousHash[prevIndex] = EndOfChain;
          previousExact[prevIndex] = EndOfChain;
          continue;
        }

        // build hash chain, i.e. store distance to last pseudo-match
        previousHash[prevIndex] = (Distance)distance;

        // skip pseudo-matches (hash collisions) and build a second chain where
        // the first four bytes must match exactly
        uint32_t currentFour;
        // check the hash chain
        while (true) {
          // read four bytes
          currentFour = READ32LE(
              &data[lastHashMatch - dataZero]);  // match may be found in the
                                                 // previous block, too
          // match chain found, first 4 bytes are identical
          if (currentFour == four) break;

          // prevent from accidently hopping on an old, wrong hash chain
          if (hash != getHash32(currentFour)) break;

          // try next pseudo-match
          Distance next = previousHash[lastHashMatch & MaxDistance];
          // end of the hash chain ?
          if (next == EndOfChain) break;

          // too far away ?
          distance += next;
          if (distance > MaxDistance) break;

          // take another step along the hash chain ...
          lastHashMatch -= next;
          // closest match is out of range ?
          if (lastHashMatch < dataZero) break;
        }

        // search aborted / failed ?
        if (four != currentFour) {
          // no matches for the first four bytes
          previousExact[prevIndex] = EndOfChain;
          continue;
        }

        // store distance to previous match
        previousExact[prevIndex] = (Distance)distance;

        // no matching if crossing block boundary, just update hash tables
        if (i < 0) continue;

        // skip match finding if in greedy mode
        if (skipMatches > 0) {
          skipMatches--;
          if (!lazyEvaluation) continue;
          lazyEvaluation = false;
        }

        // and after all that preparation ... finally look for the longest match
        matches[i] = findLongestMatch(data.data(), i + lastBlock, dataZero,
                                      nextBlock - BlockEndLiterals,
                                      previousExact.data());

        // no match finding needed for the next few bytes in greedy/lazy mode
        if ((isLazy || isGreedy) && matches[i].length != JustLiteral) {
          lazyEvaluation = (skipMatches == 0);
          skipMatches = matches[i].length;
        }
      }
      // last bytes are always literals
      while (i < int(matches.size())) matches[i++].length = JustLiteral;

      // dictionary is valid only to the first block
      parseDictionary = false;

      // ==================== estimate costs (number of compressed bytes)
      // ====================

      // not needed in greedy mode and/or very short blocks
      if (matches.size() > BlockEndNoMatch &&
          maxChainLength > ShortChainsGreedy)
        estimateCosts(matches);

      // ==================== select best matches ====================

      std::vector<unsigned char> compressed =
          selectBestMatches(matches, &data[lastBlock - dataZero]);

      // ==================== output ====================

      // did compression do harm ?
      bool useCompression = compressed.size() < blockSize && !uncompressed;
      // legacy format is always compressed
      useCompression |= useLegacyFormat;

      // block size
      uint32_t numBytes =
          uint32_t(useCompression ? compressed.size() : blockSize);
      uint32_t numBytesTagged = numBytes | (useCompression ? 0 : 0x80000000);
      unsigned char num1 = numBytesTagged & 0xFF;
      sendBytes(&num1, 1, userPtr);
      unsigned char num2 = (numBytesTagged >> 8) & 0xFF;
      sendBytes(&num2, 1, userPtr);
      unsigned char num3 = (numBytesTagged >> 16) & 0xFF;
      sendBytes(&num3, 1, userPtr);
      unsigned char num4 = (numBytesTagged >> 24) & 0xFF;
      sendBytes(&num4, 1, userPtr);

      if (useCompression)
        sendBytes(compressed.data(), numBytes, userPtr);
      else  // uncompressed ? => copy input data
        sendBytes(&data[lastBlock - dataZero], numBytes, userPtr);

      // legacy format: no matching across blocks
      if (useLegacyFormat) {
        dataZero += data.size();
        data.clear();

        // clear hash tables
        for (size_t i = 0; i < previousHash.size(); i++)
          previousHash[i] = EndOfChain;
        for (size_t i = 0; i < previousExact.size(); i++)
          previousExact[i] = EndOfChain;
        for (size_t i = 0; i < lastHash.size(); i++) lastHash[i] = NoLastHash;
      } else {
        // remove already processed data except for the last 64kb which could be
        // used for intra-block matches
        if (data.size() > MaxDistance) {
          size_t remove = data.size() - MaxDistance;
          dataZero += remove;
          data.erase(data.begin(), data.begin() + remove);
        }
      }
    }

    // add an empty block
    if (!useLegacyFormat) {
      static const uint32_t zero = 0;
      sendBytes(&zero, 4, userPtr);
    }
  }
};

#endif /* COSMOPOLITAN_THIRD_PARTY_SMALLZ4_SMALLZ4_H_ */
