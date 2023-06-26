// clang-format off
// This file implements a multi-threaded zlib and zstd compression
// routine.
//
// zlib-compressed data can be merged just by concatenation as long as
// each piece of data is flushed with Z_SYNC_FLUSH. In this file, we
// split input data into multiple shards, compress them individually
// and concatenate them. We then append a header, a trailer and a
// checksum so that the concatenated data is valid zlib-format data.
//
// zstd-compressed data can be merged in the same way.
//
// Using threads to compress data has a downside. Since the dictionary
// is reset on boundaries of shards, compression ratio is sacrificed
// a little bit. However, if a shard size is large enough, that loss
// is negligible in practice.

#include "third_party/mold/common.h"

// MISSING #include <tbb/parallel_for_each.h>
// MISSING #include <zlib.h>
// MISSING #include <zstd.h>

#define CHECK(fn)                               \
  do {                                          \
    [[maybe_unused]] int r = (fn);              \
    assert(r == Z_OK);                          \
  } while (0)

namespace mold {

static constexpr i64 SHARD_SIZE = 1024 * 1024;

static std::vector<std::string_view> split(std::string_view input) {
  std::vector<std::string_view> shards;

  while (input.size() >= SHARD_SIZE) {
    shards.push_back(input.substr(0, SHARD_SIZE));
    input = input.substr(SHARD_SIZE);
  }
  if (!input.empty())
    shards.push_back(input);
  return shards;
}

static std::vector<u8> zlib_compress(std::string_view input) {
  // Initialize zlib stream. Since debug info is generally compressed
  // pretty well with lower compression levels, we chose compression
  // level 1.
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;

  CHECK(deflateInit2(&strm, 1, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY));

  // Set an input buffer
  strm.avail_in = input.size();
  strm.next_in = (u8 *)input.data();

  // Set an output buffer. deflateBound() returns an upper bound
  // on the compression size. +16 for Z_SYNC_FLUSH.
  std::vector<u8> buf(deflateBound(&strm, strm.avail_in) + 16);

  // Compress data. It writes all compressed bytes except the last
  // partial byte, so up to 7 bits can be held to be written to the
  // buffer.
  strm.avail_out = buf.size();
  strm.next_out = buf.data();
  CHECK(deflate(&strm, Z_BLOCK));

  // This is a workaround for libbacktrace before 2022-04-06.
  //
  // Zlib is a bit stream, and what Z_SYNC_FLUSH does is to write a
  // three bit value indicating the start of an uncompressed data
  // block followed by four byte data 00 00 ff ff which indicates that
  // the length of the block is zero. libbacktrace uses its own zlib
  // inflate routine, and it had a bug that if that particular three
  // bit value happens to end at a byte boundary, it accidentally
  // skipped the next byte.
  //
  // In order to avoid triggering that bug, we should avoid calling
  // deflate() with Z_SYNC_FLUSH if the current bit position is 5.
  // If it's 5, we insert an empty block consisting of 10 bits so
  // that the bit position is 7 in the next byte.
  //
  // https://github.com/ianlancetaylor/libbacktrace/pull/87
  int nbits;
  deflatePending(&strm, Z_NULL, &nbits);
  if (nbits == 5)
    CHECK(deflatePrime(&strm, 10, 2));
  CHECK(deflate(&strm, Z_SYNC_FLUSH));

  assert(strm.avail_out > 0);
  buf.resize(buf.size() - strm.avail_out);
  buf.shrink_to_fit();
  deflateEnd(&strm);
  return buf;
}

ZlibCompressor::ZlibCompressor(u8 *buf, i64 size) {
  std::string_view input{(char *)buf, (size_t)size};
  std::vector<std::string_view> inputs = split(input);
  std::vector<u64> adlers(inputs.size());
  shards.resize(inputs.size());

  // Compress each shard
  tbb::parallel_for((i64)0, (i64)inputs.size(), [&](i64 i) {
    adlers[i] = adler32(1, (u8 *)inputs[i].data(), inputs[i].size());
    shards[i] = zlib_compress(inputs[i]);
  });

  // Combine checksums
  checksum = adlers[0];
  for (i64 i = 1; i < inputs.size(); i++)
    checksum = adler32_combine(checksum, adlers[i], inputs[i].size());

  // Comput the total size
  compressed_size = 8; // the header and the trailer
  for (std::vector<u8> &shard : shards)
    compressed_size += shard.size();
}

void ZlibCompressor::write_to(u8 *buf) {
  // Write a zlib-format header
  buf[0] = 0x78;
  buf[1] = 0x9c;

  // Copy compressed data
  std::vector<i64> offsets(shards.size());
  offsets[0] = 2; // +2 for header
  for (i64 i = 1; i < shards.size(); i++)
    offsets[i] = offsets[i - 1] + shards[i - 1].size();

  tbb::parallel_for((i64)0, (i64)shards.size(), [&](i64 i) {
    memcpy(&buf[offsets[i]], shards[i].data(), shards[i].size());
  });

  // Write a trailer
  u8 *end = buf + compressed_size;
  end[-6] = 3;
  end[-5] = 0;

  // Write a checksum
  *(ub32 *)(end - 4) = checksum;
}

static std::vector<u8> zstd_compress(std::string_view input) {
  std::vector<u8> buf(ZSTD_COMPRESSBOUND(input.size()));
  constexpr int level = 3; // compression level; must be between 1 to 22

  size_t sz = ZSTD_compress(buf.data(), buf.size(), input.data(), input.size(),
                            level);
  assert(!ZSTD_isError(sz));
  buf.resize(sz);
  buf.shrink_to_fit();
  return buf;
}

ZstdCompressor::ZstdCompressor(u8 *buf, i64 size) {
  std::string_view input{(char *)buf, (size_t)size};
  std::vector<std::string_view> inputs = split(input);
  shards.resize(inputs.size());

  // Compress each shard
  tbb::parallel_for((i64)0, (i64)inputs.size(), [&](i64 i) {
    shards[i] = zstd_compress(inputs[i]);
  });

  compressed_size = 0;
  for (std::vector<u8> &shard : shards)
    compressed_size += shard.size();
}

void ZstdCompressor::write_to(u8 *buf) {
  // Copy compressed data
  std::vector<i64> offsets(shards.size());
  for (i64 i = 1; i < shards.size(); i++)
    offsets[i] = offsets[i - 1] + shards[i - 1].size();

  tbb::parallel_for((i64)0, (i64)shards.size(), [&](i64 i) {
    memcpy(&buf[offsets[i]], shards[i].data(), shards[i].size());
  });
}

} // namespace mold
