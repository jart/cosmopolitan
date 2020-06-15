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

static size_t g_environcap;

int PutEnvImpl(char *string, bool overwrite) {
  if (!environ) {
    g_environcap = 0;
    if ((environ = calloc(8, sizeof(char *)))) {
      g_environcap = 8;
    }
  }
  char *equalp = strchr(string, '=');
  if (!equalp) return einval();
  unsigned namelen = equalp + 1 - string;
  unsigned i;
  for (i = 0; environ[i]; ++i) {
    if (strncmp(environ[i], string, namelen) == 0) {
      if (!overwrite) {
        free_s(&string);
        return 0;
      }
      goto replace;
    }
  }
  if (i + 1 >= g_environcap) {
    if (!g_environcap) g_environcap = i + 1;
    if (!grow(&environ, &g_environcap, sizeof(char *), 0)) {
      free_s(&string);
      return -1;
    }
  }
  environ[i + 1] = NULL;
replace:
  free_s(&environ[i]);
  environ[i] = string;
  return 0;
}

/**
 * Emplaces environment key=value.
 * @see setenv(), getenv()
 */
int putenv(char *string) { return PutEnvImpl(string, true); }

textexit static void putenv_fini(void) {
  for (char **envp = environ; *envp; ++envp) free_s(envp);
  free_s(&environ);
}

textstartup static void putenv_init(void) { atexit(putenv_fini); }

const void *const putenv_ctor[] initarray = {putenv_init};
