/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"

#define _TLSZ ((intptr_t)_tls_size)
#define _TLDZ ((intptr_t)_tdata_size)
#define _TIBZ sizeof(struct cthread_descriptor_t)
#define _MEMZ ROUNDUP(_TLSZ + _TIBZ, alignof(struct cthread_descriptor_t))

/**
 * Allocates thread-local storage memory for new thread.
 * @return buffer that must be released with free()
 */
char *_mktls(char **out_tib) {
  char *tls;
  cthread_t tib;

  // Allocate enough TLS memory for all the GNU Linuker (_tls_size)
  // organized _Thread_local data, as well as Cosmpolitan Libc (64)
  if (!(tls = calloc(1, _MEMZ))) return 0;

  // set up thread informaiton block
  tib = (cthread_t)(tls + _MEMZ - _TIBZ);
  tib->self = tib;
  tib->self2 = tib;
  tib->err = 0;
  tib->tid = -1;
  memmove(tls, _tdata_start, _TLDZ);

  if (out_tib) {
    *out_tib = (char *)tib;
  }
  return tls;
}
