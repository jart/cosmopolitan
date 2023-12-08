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
#include "libc/errno.h"
#include "libc/intrin/describentoverlapped.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"

__msabi extern typeof(WSAGetOverlappedResult)
    *const __imp_WSAGetOverlappedResult;

static char *DescribeTransfer(char buf[16], bool32 ok, uint32_t *transfer) {
  if (ok) {
    ksnprintf(buf, 16, "%'u", *transfer);
    return buf;
  } else {
    return "n/a";
  }
}

/**
 * Gets overlap i/o result.
 *
 * @return true on success, or false on failure
 * @note this wrapper takes care of ABI, STRACE(), and __winsockerr()
 */
bool32 WSAGetOverlappedResult(uint64_t s,
                              const struct NtOverlapped *lpOverlapped,
                              uint32_t *out_lpcbTransfer, bool32 fWait,
                              uint32_t *out_lpdwFlags) {
  bool32 ok;
  ok = __imp_WSAGetOverlappedResult(s, lpOverlapped, out_lpcbTransfer, fWait,
                                    out_lpdwFlags);
  if (!ok) __winsockerr();
  NTTRACE("WSAGetOverlappedResult(%ld, %s, [%s], %hhhd, [%#x]) → %hhhd% lm", s,
          DescribeNtOverlapped(lpOverlapped),
          DescribeTransfer(alloca(16), ok, out_lpcbTransfer), fWait,
          *out_lpdwFlags, ok);
  return ok;
}
