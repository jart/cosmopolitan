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
#include "libc/bits/weaken.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "test/tool/build/lib/numbers.h"
#include "test/tool/build/lib/optest.h"
#include "tool/build/lib/flags.h"

const char kOpSuffix[] = {'b', 'w', 'l', 'q'};

void(RunOpTests)(const uint8_t *ops, size_t n, const char *const *opnames,
                 const char *file, int line, const char *func) {
  uint64_t x, y;
  uint64_t xn, xp;
  uint32_t f0, f1, f2;
  long failed, succeeded;
  int w, h, i, j, c, z, s, o, p;
  failed = 0;
  succeeded = 0;
  for (w = 0; w < 4; ++w) {
    for (h = 0; h < n; ++h) {
      for (z = 0; z < 2; ++z) {
        for (o = 0; o < 2; ++o) {
          for (s = 0; s < 2; ++s) {
            for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
              for (j = 0; j < ARRAYLEN(kNumbers); ++j) {
                for (c = 0; c < 2; ++c) {
                  x = kNumbers[i];
                  y = kNumbers[j];
                  f2 = f1 = f0 = c << FLAGS_CF | z << FLAGS_ZF | s << FLAGS_SF |
                                 o << FLAGS_OF;
                  xn = RunGolden(w, ops[h], x, y, &f1);
                  xp = RunOpTest(w, ops[h], x, y, &f2);
                  if (weaken(FixupUndefOpTestFlags)) {
                    FixupUndefOpTestFlags(w, ops[h], x, y, f1, &f2);
                  }
                  if (xn == xp && (f1 & FMASK) == (f2 & FMASK)) {
                    succeeded++;
                  } else if (failed++ < 10) {
                    fprintf(stderr,
                            "%s:%d:%s: %s%c failed\n\t"
                            "𝑥  %016x\n\t"
                            "𝑦  %016x %c%c%c%c 0NPlODITSZKA1PVC\n\t"
                            "𝑥ₙ %016x %c%c%c%c %016b\n\t"
                            "𝑥ₚ %016x %c%c%c%c %016b\n",
                            file, line, func, opnames[ops[h]], kOpSuffix[w], x,
                            y, ".C"[c], ".Z"[z], ".S"[s], ".O"[o], xn,
                            ".C"[!!(f1 & (1 << FLAGS_CF))],
                            ".Z"[!!(f1 & (1 << FLAGS_ZF))],
                            ".S"[!!(f1 & (1 << FLAGS_SF))],
                            ".O"[!!(f1 & (1 << FLAGS_OF))], f1, xp,
                            ".C"[!!(f2 & (1 << FLAGS_CF))],
                            ".Z"[!!(f2 & (1 << FLAGS_ZF))],
                            ".S"[!!(f2 & (1 << FLAGS_SF))],
                            ".O"[!!(f2 & (1 << FLAGS_OF))], f2);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if (failed) {
    fprintf(stderr,
            "\n"
            "passing:   %d%%\n"
            "succeeded: %,ld\n"
            "failed:    %,ld\n"
            "\n",
            (int)((1 - (double)failed / succeeded) * 100), succeeded, failed);
    exit(1);
  }
}
