/* header created automatically with -DGEN_TREES_H */
#include "third_party/zlib/internal.h"

const int kZlibBaseLength[LENGTH_CODES] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,   10,  12,  14,  16,  20, 24,
    28, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 0,
};

const int kZlibBaseDist[D_CODES] = {
    0,    1,    2,    3,    4,    6,    8,    12,    16,    24,
    32,   48,   64,   96,   128,  192,  256,  384,   512,   768,
    1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576,
};
