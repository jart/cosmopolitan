/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/rand/lcg.internal.h"
#include "libc/rand/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define RESEED   1024
#define ATTEMPTS 10
#define WILDCARD "XXXXXX"

int mkostempsmi(char *tpl, int slen, unsigned flags, uint64_t *rando, int mode,
                int openit(const char *file, int flags, ...)) {
  size_t len = strlen(tpl);
  size_t wildlen = strlen(WILDCARD);
  if (len < wildlen || slen > len - wildlen) {
    return einval();
  }
  char *ss = tpl + len - wildlen - slen;
  assert(memcmp(ss, WILDCARD, wildlen) == 0);
  flags = (flags & ~(flags & O_ACCMODE)) | O_RDWR | O_CREAT | O_EXCL;
  unsigned attempts = ATTEMPTS;
  do {
    char *p = ss;
    uint32_t num = KnuthLinearCongruentialGenerator(rando) >> 32;
    for (unsigned i = 0; i < wildlen; ++i) {
      p[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[num & 31];
      num >>= 5;
    }
    int fd;
    if ((fd = openit(tpl, flags, 0600)) != -1) return fd;
  } while (--attempts && errno == EEXIST);
  memcpy(ss, WILDCARD, wildlen);
  return -1;
}

static uint64_t g_mkostemps_rand;
static uint64_t g_mkostemps_reseed;

/**
 * Opens unique temporary file.
 *
 * The substring XXXXXX is replaced with a pseudorandom number that's
 * seeded automatically and grants 30 bits of randomness to each value.
 * Retries are made in the unlikely event of collisions.
 *
 * @param template is a pathname relative to current directory by default,
 *     that needs to have "XXXXXX" at the end of the string
 * @param suffixlen may be nonzero to permit characters after the XXXXXX
 * @param flags can have O_APPEND, O_CLOEXEC, etc.
 * @param mode is conventionally 0600, for owner-only non-exec access
 * @return exclusive open file descriptor for generated pathname,
 *     or -1 w/ errno
 * @see kTmpPath
 */
nodiscard int mkostempsm(char *template, int suffixlen, unsigned flags,
                         int mode) {
  if (g_mkostemps_reseed++ % RESEED == 0) g_mkostemps_rand = rand64();
  return mkostempsmi(template, suffixlen, flags, &g_mkostemps_rand, mode, open);
}
