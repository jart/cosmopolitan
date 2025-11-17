// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/sysv/pib.h"
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/thread/tls.h"

struct CosmoPib __pib;

/**
 * Returns process information block.
 *
 * The main purpose of this function is to support vfork() on Windows.
 */
__privileged struct CosmoPib *__get_pib(void) {
  if (LIKELY(!IsWindows()))
    return &__pib;
  if (__tls_enabled) {
    struct CosmoTib *tib = __get_tls_privileged();
    if (tib->tib_vfork)
      return tib->tib_vfork;
  }
  return &__pib;
}
