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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/tls.h"
#include "libc/nt/thunk/msabi.h"

__msabi extern typeof(TlsFree) *const __imp_TlsFree;
__msabi extern typeof(TlsAlloc) *const __imp_TlsAlloc;
__msabi extern typeof(TlsGetValue) *const __imp_TlsGetValue;
__msabi extern typeof(TlsSetValue) *const __imp_TlsSetValue;

/**
 * Assigns thread-local storage slot.
 *
 * This function may for instance be called at startup and the result
 * can be assigned to a global static variable; from then on, all the
 * threads in your application may pass that value to TlsGetValue, to
 * retrieve their thread-local values.
 *
 * @return index on success, or -1u w/ errno
 * @threadsafe
 */
uint32_t TlsAlloc(void) {
  return __imp_TlsAlloc();
}

/**
 * Releases thread-local storage slot.
 * @threadsafe
 */
bool32 TlsFree(uint32_t dwTlsIndex) {
  return __imp_TlsFree(dwTlsIndex);
}

/**
 * Sets value to thread-local storage slot.
 *
 * @param dwTlsIndex is something returned by TlsAlloc()
 * @return true if successful, otherwise false
 * @threadsafe
 */
bool32 TlsSetValue(uint32_t dwTlsIndex, void *lpTlsValue) {
  assert(IsWindows());
  if (dwTlsIndex < 64) {
    asm("mov\t%1,%%gs:%0"
        : "=m"(*((long *)0x1480 + dwTlsIndex))
        : "r"(lpTlsValue));
    return true;
  } else {
    return __imp_TlsSetValue(dwTlsIndex, lpTlsValue);
  }
}

/**
 * Retrieves value from thread-local storage slot.
 *
 * @param dwTlsIndex is something returned by TlsAlloc()
 * @return true if successful, otherwise false
 * @threadsafe
 */
void *TlsGetValue(uint32_t dwTlsIndex) {
  void *lpTlsValue;
  assert(IsWindows());
  if (dwTlsIndex < 64) {
    asm("mov\t%%gs:%1,%0"
        : "=r"(lpTlsValue)
        : "m"(*((long *)0x1480 + dwTlsIndex)));
    return lpTlsValue;
    // // this could also be written as...
    // asm("movq\t%%gs:0x30,%0" : "=a"(tib));
    // return (void *)tib[0x1480 / 8 + dwTlsIndex];
  } else {
    return __imp_TlsGetValue(dwTlsIndex);
  }
}
