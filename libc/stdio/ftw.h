#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_
#include "libc/calls/struct/stat.h"
COSMOPOLITAN_C_START_

/**
 * Type for file.
 */
#define FTW_F 1

/**
 * Type for directory.
 */
#define FTW_D 2

/**
 * Type for directory that cannot be read.
 */
#define FTW_DNR 3

/**
 * Type for stat() failed and not a symbolic link.
 */
#define FTW_NS 4

/**
 * Type for symbolic link when `FTW_PHYS` is in flags.
 */
#define FTW_SL 5

/**
 * Directory and `FTW_DEPTH` in flags.
 */
#define FTW_DP 6

/**
 * Type for broken symbolic link when `FTW_PHYS` is not in flags.
 */
#define FTW_SLN 7

/**
 * Flag to prevent following symbolic links (recommended).
 * @see nftw() flags
 */
#define FTW_PHYS 1

/**
 * Flag to prevent crossing mount points.
 * @see nftw() flags
 */
#define FTW_MOUNT 2

/**
 * Unsupported.
 * @see nftw() flags
 */
#define FTW_CHDIR 4

/**
 * Flag for post-order traversal.
 *
 * 1. Will use `FTW_DP` instead of `FTW_D` as type.
 * 2. Directory callback happens *after* rather than before.
 *
 * @see nftw() flags
 */
#define FTW_DEPTH 8

struct FTW {

  /**
   * Byte offset of basename component in `fpath` passed to callback.
   */
  int base;

  /**
   * Depth relative to `dirpath` whose level is zero.
   */
  int level;
};

int ftw(const char *, int (*)(const char *, const struct stat *, int),
        int) dontthrow;

int nftw(const char *,
         int (*)(const char *, const struct stat *, int, struct FTW *), int,
         int) dontthrow;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_FTW_H_ */
