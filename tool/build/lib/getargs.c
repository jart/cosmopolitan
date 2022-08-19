/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "tool/build/lib/getargs.h"

/**
 * @fileoverview Fast Command Line Argument Ingestion.
 *
 * The purpose of this library is to be able to have build commands with
 * huge argument lists. The way we do that is by replacing commands like
 *
 *     foo.com lots of args
 *
 * with this
 *
 *     echo of args >args
 *     foo.com lots @args
 *
 * This iterator abstracts the process of reading the special `@`
 * prefixed args. In order to do that quickly and easily, we make the
 * following assumptions:
 *
 * 1. Arguments don't have whitespace.
 * 2. Files have a trailing whitespace.
 *
 * We need (1) so GNU Make can go faster. Assume we tokenized based on
 * newlines. We would would write that in our Makefile as follows:
 *
 *     # don't do this
 *     target: thousands of args
 *         $(file >$@.args) $(foreach x,$^,$(file >>$@.args,$(x)))
 *         tool.com -o $@ @$@.args
 *
 * That is slow because it needs to open and close the args file
 * thousands of times. If we trade away filenames with spaces then the
 * following will only require a couple system calls:
 *
 *     # do this
 *     target: thousands of args
 *         $(file >$@.args,$^)
 *         tool.com -o $@ @$@.args
 *
 * We need (2) because it make the code in this file simpler and avoids
 * a malloc() dependency. Having that trailing character means argument
 * parsing from files can be a zero-copy operation.
 */

#define IsSpace(c) ((255 & (c)) <= ' ')

/**
 * Zeroes GetArgs object and sets its fields.
 * @param args is borrowed for the lifetime of the GetArgs object
 */
void getargs_init(struct GetArgs *ga, char **args) {
  assert(args);
  bzero(ga, sizeof(*ga));
  ga->args = args;
}

/**
 * Releases memory associated with GetArgs object and zeroes it.
 */
void getargs_destroy(struct GetArgs *ga) {
  if (ga->map) munmap(ga->map, ga->mapsize);
  bzero(ga, sizeof(*ga));
}

/**
 * Gets next argument, e.g.
 *
 *     const char *s;
 *     while ((s = getargs_next(&ga))) {
 *       printf("%s\n", s);
 *     }
 *
 * @return NUL-terminated string; it should not be freed; it should be
 *     assumed that it stays in scope until the next getargs_next call
 */
const char *getargs_next(struct GetArgs *ga) {
  int fd;
  char *p;
  size_t k;
  unsigned m;
  struct stat st;
  do {
    if (ga->map) {
      for (; ga->j < ga->mapsize; ++ga->j) {
        if (!IsSpace(ga->map[ga->j])) {
          break;
        }
      }
      k = 0;
#if defined(__SSE2__) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
      typedef unsigned char xmm_t
          __attribute__((__vector_size__(16), __aligned__(1)));
      for (; ga->j + k + 16 <= ga->mapsize; k += 16) {
        if ((m = __builtin_ia32_pmovmskb128(
                     *(const xmm_t *)(ga->map + ga->j + k) >
                     (xmm_t){' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                             ' ', ' ', ' ', ' ', ' ', ' '}) ^
                 0xffff)) {
          k += __builtin_ctzl(m);
          break;
        }
      }
#endif
      for (; ga->j + k < ga->mapsize; ++k) {
        if (IsSpace(ga->map[ga->j + k])) {
          break;
        }
      }
      if (k) {
        if (ga->j + k < ga->mapsize) {
          ga->map[ga->j + k] = 0;
          p = ga->map + ga->j;
          ga->j += ++k;
          return p;
        } else {
          eio();
          break;
        }
      }
      if (munmap(ga->map, ga->mapsize) == -1) break;
      ga->map = 0;
      ga->mapsize = 0;
      ga->j = 0;
    }
    if (!(p = ga->args[ga->i])) return 0;
    ++ga->i;
    if (*p != '@') return p;
    ++p;
    if ((fd = open((ga->path = p), O_RDONLY)) != -1) {
      fstat(fd, &st);
      if ((p = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd,
                    0)) != MAP_FAILED) {
        ga->map = p;
        ga->mapsize = st.st_size;
      }
      close(fd);
    }
  } while (ga->map);
  perror(ga->path);
  exit(1);
}
