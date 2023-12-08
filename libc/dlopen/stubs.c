/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dlopen/dlfcn.h"

/**
 * Opens dynamic shared object using host platform libc.
 *
 * Cosmo's `dlopen()` function is a stub that always returns NULL. An
 * alternative `cosmo_dlopen()` implementation is provided, which can
 * help some users with tasks like talking to GPU and GUI libraries.
 *
 * @return null always
 */
void *dlopen(const char *, int) {
  return 0;
}

char *dlerror(void) {
  return "dlopen() isn't supported by cosmo; try using cosmo_dlopen()";
}

void *dlsym(void *, const char *) {
  return 0;
}

int dlclose(void *) {
  return -1;
}

int dl_iterate_phdr(int (*)(void *, size_t, void *), void *) {
  return -1;
}
