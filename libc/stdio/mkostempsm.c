/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/rand/lcg.h"
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
