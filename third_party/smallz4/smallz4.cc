/*-*- mode:c++;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8 -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  smallz4                                                                     │
│  Copyright (c) 2016-2019 Stephan Brumme. All rights reserved.                │
│  See https://create.stephan-brumme.com/smallz4/                              │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/time/time.h"
#include "third_party/smallz4/smallz4.hh"

/// error handler
static void error(const char* msg, int code = 1) {
  fprintf(stderr, "ERROR: %s\n", msg);
  exit(code);
}

// ==================== user-specific I/O INTERFACE ====================

struct UserPtr {
  // file handles
  FILE* in;
  FILE* out;
  // the attributes below are just needed for verbose output
  bool verbose;
  uint64_t numBytesIn;
  uint64_t numBytesOut;
  uint64_t totalSize;
  time_t starttime;
};

/// read several bytes and store at "data", return number of actually read bytes
/// (return only zero if end of data reached)
size_t getBytesFromIn(void* data, size_t numBytes, void* userPtr) {
  /// cast user-specific data
  UserPtr* user = (UserPtr*)userPtr;

  if (data && numBytes > 0) {
    size_t actual = fread(data, 1, numBytes, user->in);
    user->numBytesIn += actual;

    return actual;
  }
  return 0;
}

/// show verbose info on STDERR
void verbose(const UserPtr& user) {
  if (!user.verbose) return;
  if (user.numBytesIn == 0) return;

  // elapsed and estimated time in seconds
  int duration = int(time(NULL) - user.starttime);
  if (duration == 0) return;
  int estimated = int(duration * user.totalSize / user.numBytesIn);

  // display on STDERR
  fprintf(stderr, "\r%lld bytes => %lld bytes (%d%%", user.numBytesIn,
          user.numBytesOut, 100 * user.numBytesOut / user.numBytesIn);
  if (estimated > 0) fprintf(stderr, ", %d%% done", 100 * duration / estimated);
  fprintf(stderr, "), after %d seconds @ %d kByte/s", duration,
          duration > 0 ? (user.numBytesIn / duration) / 1024 : 0);
  if (estimated > 0)
    fprintf(stderr, ", about %d seconds left  ", estimated - duration);
}

/// write a block of bytes
void sendBytesToOut(const void* data, size_t numBytes, void* userPtr) {
  /// cast user-specific data
  UserPtr* user = (UserPtr*)userPtr;
  if (data && numBytes > 0) {
    fwrite(data, 1, numBytes, user->out);
    user->numBytesOut += numBytes;

    if (user->verbose) verbose(*user);
  }
}

// ==================== COMMAND-LINE HANDLING ====================

// show simple help
static void showHelp(const char* program) {
  printf(
      "smalLZ4 %s%s: compressor with optimal parsing, fully compatible with "
      "LZ4 by Yann Collet (see https://lz4.org)\n"
      "\n"
      "Basic usage:\n"
      "  %s [flags] [input] [output]\n"
      "\n"
      "This program writes to STDOUT if output isn't specified\n"
      "and reads from STDIN if input isn't specified, either.\n"
      "\n"
      "Examples:\n"
      "  %s   < abc.txt > abc.txt.lz4    # use STDIN and STDOUT\n"
      "  %s     abc.txt > abc.txt.lz4    # read from file and write to STDOUT\n"
      "  %s     abc.txt   abc.txt.lz4    # read from and write to file\n"
      "  cat abc.txt | %s - abc.txt.lz4  # read from STDIN and write to file\n"
      "  %s -6  abc.txt   abc.txt.lz4    # compression level 6 (instead of "
      "default 9)\n"
      "  %s -f  abc.txt   abc.txt.lz4    # overwrite an existing file\n"
      "  %s -f7 abc.txt   abc.txt.lz4    # compression level 7 and overwrite "
      "an existing file\n"
      "\n"
      "Flags:\n"
      "  -0, -1 ... -9   Set compression level, default: 9 (see below)\n"
      "  -h              Display this help message\n"
      "  -f              Overwrite an existing file\n"
      "  -l              Use LZ4 legacy file format\n"
      "  -D [FILE]       Load dictionary\n"
      "  -v              Verbose\n"
      "\n"
      "Compression levels:\n"
      " -0               No compression\n"
      " -1 ... -%d        Greedy search, check 1 to %d matches\n"
      " -%d ... -8        Lazy matching with optimal parsing, check %d to 8 "
      "matches\n"
      " -9               Optimal parsing, check all possible matches "
      "(default)\n"
      "\n"
      "Written in 2016-2020 by Stephan Brumme "
      "https://create.stephan-brumme.com/smallz4/\n",
      smallz4::getVersion(), "", program, program, program, program, program,
      program, program, program, smallz4::ShortChainsGreedy,
      smallz4::ShortChainsGreedy, smallz4::ShortChainsGreedy + 1,
      smallz4::ShortChainsGreedy + 1);
}

/// parse command-line
int main(int argc, const char* argv[]) {
  // show help if no parameters and stdin isn't a pipe
  if (argc == 1 && isatty(fileno(stdin)) != 0) {
    showHelp(argv[0]);
    return 0;
  }

  unsigned short maxChainLength =
      65535;  // "unlimited" because search window contains only 2^16 bytes

  // overwrite output ?
  bool overwrite = false;
  // legacy format ? (not recommended, but smaller files if input < 8 MB)
  bool useLegacy = false;
  // preload dictionary from disk
  const char* dictionary = NULL;

  // default input/output streams
  UserPtr user;
  user.in = stdin;
  user.out = stdout;
  user.verbose = false;
  user.numBytesIn = 0;
  user.numBytesOut = 0;
  user.totalSize = 0;

  // parse flags
  int nextArgument = 1;
  bool skipArgument = false;
  while (argc > nextArgument && argv[nextArgument][0] == '-') {
    int argPos = 1;
    while (argv[nextArgument][argPos] != '\0') {
      switch (argv[nextArgument][argPos++]) {
          // show help
        case 'h':
          showHelp(argv[0]);
          return 0;

          // force overwrite
        case 'f':
          overwrite = true;
          break;

          // old LZ4 format
        case 'l':
          useLegacy = true;
          break;

          // use dictionary
        case 'D':
          if (nextArgument + 1 >= argc) error("no dictionary filename found");
          dictionary =
              argv[nextArgument +
                   1];  // TODO: any flag immediately after -D causes an error
          skipArgument = true;
          break;

          // display some info on STDERR while compressing
        case 'v':
          user.verbose = true;
          break;

          // set compression level
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
          maxChainLength =
              argv[nextArgument][1] - '0';  // "0" => 0, "1" => 1, ..., "8" => 8
          break;

          // unlimited hash chain length
        case '9':
          // default maxChainLength is already "unlimited"
          break;

        default:
          error("unknown flag");
      }
    }

    nextArgument++;
    if (skipArgument) nextArgument++;
  }

  // input file is given as first parameter or stdin if no parameter is given
  // (or "-")
  if (argc > nextArgument && argv[nextArgument][0] != '-') {
    user.in = fopen(argv[nextArgument], "rb");
    if (!user.in) error("file not found");
    nextArgument++;
  }

  // output file is given as second parameter or stdout if no parameter is given
  // (or "-")
  if (argc == nextArgument + 1 && argv[nextArgument][0] != '-') {
    // check if file already exists
    if (!overwrite && fopen(argv[nextArgument], "rb"))
      error("output file already exists");

    user.out = fopen(argv[nextArgument], "wb");
    if (!user.out) error("cannot create file");
  }

  // basic check of legacy format's restrictions
  if (useLegacy) {
    if (dictionary != 0) error("legacy format doesn't support dictionaries");
    if (maxChainLength == 0)
      error("legacy format doesn't support uncompressed files");
  }

  // load dictionary
  std::vector<unsigned char> preload;
  if (dictionary != NULL) {
    // open dictionary
    FILE* dict = fopen(dictionary, "rb");
    if (!dict) error("cannot open dictionary");

    // get dictionary's filesize
    fseek(dict, 0, SEEK_END);
    size_t dictSize = ftell(dict);
    // only the last 64k are relevant
    const size_t Last64k = 65536;
    size_t relevant = dictSize < Last64k ? 0 : dictSize - Last64k;
    fseek(dict, (long)relevant, SEEK_SET);
    if (dictSize > Last64k) dictSize = Last64k;

    // read those bytes
    preload.resize(dictSize);
    fread(&preload[0], 1, dictSize, dict);
    fclose(dict);
  }

  if (user.verbose) {
    if (user.in != stdin) {
      fseek(user.in, 0, SEEK_END);
      user.totalSize = ftell(user.in);
      fseek(user.in, 0, SEEK_SET);
    }

    user.starttime = time(NULL);
  }

  // and go !
  smallz4::lz4(getBytesFromIn, sendBytesToOut, maxChainLength, preload,
               useLegacy, &user);

  if (user.verbose && user.numBytesIn > 0)
    fprintf(stderr,
            "\r%lld bytes => %lld bytes (%d%%) after %d seconds                "
            "                                                      \n",
            user.numBytesIn, user.numBytesOut,
            100 * user.numBytesOut / user.numBytesIn,
            int(time(NULL) - user.starttime));

  return 0;
}
