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
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/musl/ftw.h"

/**
 * @fileoverview Directory walker example.
 * Copied from IEEE Std 1003.1-2017
 */

static int display_info(const char *fpath, const struct stat *sb, int tflag,
                        struct FTW *ftwbuf) {
  printf("%-3s %2d %7jd   %-40s %d %s\n",
         (tflag == FTW_D)     ? "d"
         : (tflag == FTW_DNR) ? "dnr"
         : (tflag == FTW_DP)  ? "dp"
         : (tflag == FTW_F)   ? (S_ISBLK(sb->st_mode)    ? "f b"
                                 : S_ISCHR(sb->st_mode)  ? "f c"
                                 : S_ISFIFO(sb->st_mode) ? "f p"
                                 : S_ISREG(sb->st_mode)  ? "f r"
                                 : S_ISSOCK(sb->st_mode) ? "f s"
                                                         : "f ?")
         : (tflag == FTW_NS)  ? "ns"
         : (tflag == FTW_SL)  ? "sl"
         : (tflag == FTW_SLN) ? "sln"
                              : "?",
         ftwbuf->level, (intmax_t)sb->st_size, fpath, ftwbuf->base,
         fpath + ftwbuf->base);
  return 0; /* To tell nftw() to continue */
}

int main(int argc, char *argv[]) {
  int flags = 0;
  if (argc > 2 && strchr(argv[2], 'd') != NULL) flags |= FTW_DEPTH;
  if (argc > 2 && strchr(argv[2], 'p') != NULL) flags |= FTW_PHYS;
  if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
    perror("nftw");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
