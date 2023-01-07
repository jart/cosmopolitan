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
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "usage: %s FOO.COM\n", argv[0]);
    fprintf(stderr,
            "prints lowest file offset (in decimal bytes) at which zip\n"
            "records exist within a .com, .com.dbg, or .zip file\n");
    exit(1);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror(argv[1]);
    exit(1);
  }

  size_t n;
  struct stat st;
  CHECK_EQ(0, fstat(fd, &st));
  n = st.st_size;

  uint8_t *m;
  CHECK_NE(MAP_FAILED, (m = mmap(0, n, PROT_READ, MAP_PRIVATE, fd, 0)));

  uint8_t *b, *d, *p;
  if ((p = FindEmbeddedApe(m, n))) {
    b = p;
    n -= p - m;
  } else {
    b = m;
  }
  if (!(d = GetZipCdir(b, n))) {
    fprintf(stderr, "%s: couldn't locate central directory\n", argv[1]);
    exit(1);
  }

  size_t zsize = n;
  uint8_t *zmap = m;
  uint8_t *zbase = b;
  uint8_t *zcdir = d;
  DCHECK(IsZipCdir32(zbase, zsize, zcdir - zbase) ||
         IsZipCdir64(zbase, zsize, zcdir - zbase));

  uint64_t cf;
  uint64_t lf;
  uint64_t minzipoffset;
  n = GetZipCdirRecords(zcdir);
  minzipoffset = cf = GetZipCdirOffset(zcdir);
  for (; n--; cf += ZIP_CFILE_HDRSIZE(zbase + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zbase + cf));
    lf = GetZipCfileOffset(zbase + cf);
    minzipoffset = MIN(minzipoffset, lf);
  }

  printf("%ld\n", (zbase + minzipoffset) - m);
}
