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
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

/**
 * Returns auxiliary value, or zero if kernel didn't provide it.
 *
 * This function is typically regarded as a libc implementation detail;
 * thus, the source code is the documentation.
 *
 * @see libc/sysv/consts.sh
 * @see System Five Application Binary Interface § 3.4.3
 * @asyncsignalsafe
 */
unsigned long getauxval(unsigned long at) {
  unsigned long res = 0;
  if (at != -1) {
    if (!IsWindows()) {
      if (!g_auxv) return 0;
      if (IsXnu()) {
        if (at) {
          const char *name =
              at == AT_EXECFN ? "executable_path" : (const char *)at;
          const char **auxv = (const char **)g_auxv;
          unsigned namelen = strlen(name);
          for (int i = 0; auxv[i]; ++i) {
            if (strncmp(auxv[i], name, namelen) == 0 &&
                auxv[i][namelen] == '=') {
              res = (intptr_t)&auxv[i][namelen + 1];
              break;
            }
          }
        }
      } else {
        for (const unsigned long *ap = g_auxv; *ap; ap += 2) {
          if (ap[0] == at) {
            res = ap[1];
            break;
          }
        }
      }
    } else {
      /* TODO(jart): GetProcessImageFileName */
    }
    if (res == 0 && at == AT_EXECFN) {
      res = (uintptr_t)program_invocation_name;
    }
  }
  return res;
}
