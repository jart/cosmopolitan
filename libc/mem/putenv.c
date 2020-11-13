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
#include "libc/alg/alg.h"
#include "libc/mem/internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

#define MAX_VARS 512

int PutEnvImpl(char *s, bool overwrite) {
  char *p;
  unsigned i, namelen;
  p = strchr(s, '=');
  if (!p) goto fail;
  namelen = p + 1 - s;
  for (i = 0; environ[i]; ++i) {
    if (strncmp(environ[i], s, namelen) == 0) {
      if (!overwrite) {
        free(s);
        return 0;
      }
      goto replace;
    }
  }
  if (i + 1 >= MAX_VARS) goto fail;
  environ[i + 1] = NULL;
replace:
  free(environ[i]);
  environ[i] = s;
  return 0;
fail:
  free(s);
  return einval();
}

/**
 * Emplaces environment key=value.
 * @see setenv(), getenv()
 */
int putenv(char *string) {
  return PutEnvImpl(strdup(string), true);
}

textstartup static void putenv_init(void) {
  char **pin, **pout;
  pin = environ;
  pout = malloc(sizeof(char *) * MAX_VARS);
  environ = pout;
  while (*pin) *pout++ = strdup(*pin++);
  *pout = NULL;
}

const void *const putenv_ctor[] initarray = {putenv_init};
