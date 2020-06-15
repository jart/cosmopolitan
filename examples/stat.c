#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"

/**
 * @fileoverview File metadata viewer.
 *
 * This demonstrates the more powerful aspects of the printf() DSL.
 */

void PrintFileMetadata(const char *pathname, struct stat *st) {
  printf("\n%s:", pathname);
  if (stat(pathname, st) != -1) {
    printf(
        "\n"
        "%-32s%,ld\n"
        "%-32s%,ld\n"
        "%-32s%#lx\n"
        "%-32s%#lx\n"
        "%-32s%ld\n"
        "%-32s%#o\n"
        "%-32s%d\n"
        "%-32s%d\n"
        "%-32s%d\n"
        "%-32s%ld\n"
        "%-32s%s\n"
        "%-32s%s\n"
        "%-32s%s\n",
        "bytes in file", st->st_size, "physical bytes", st->st_blocks * 512,
        "device id w/ file", st->st_dev, "inode", st->st_ino, "hard link count",
        st->st_nlink, "mode / permissions", st->st_mode, "owner id", st->st_uid,
        "group id", st->st_gid, "device id (if special)", st->st_rdev,
        "block size", st->st_blksize, "access time", gc(xiso8601(&st->st_atim)),
        "modified time", gc(xiso8601(&st->st_mtim)), "c[omplicated]time",
        gc(xiso8601(&st->st_ctim)));
  } else {
    printf(" %s\n", strerror(errno));
  }
}

int main(int argc, char *argv[]) {
  size_t i;
  struct stat st;
  for (i = 1; i < argc; ++i) {
    PrintFileMetadata(argv[i], &st);
  }
  return 0;
}
