/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/vga/vga.internal.h"

#ifdef __x86_64__

ssize_t sys_readv_vga(struct Fd *fd, const struct iovec *iov, int iovlen) {
  /*
   * NOTE: this routine is always non-blocking.
   *
   * sys_readv_metal() calls here to ask if the VGA teletypewriter has any
   * "status reports" escape sequences to send out.
   *
   * If there are no such status reports, then immediately return ≤ 0.
   * sysv_readv_metal() will then try to read from an actual input device.
   */
  size_t i, redd = 0;
  ssize_t res = 0;
  for (i = 0; i < iovlen; ++i) {
    void *input = iov[i].iov_base;
    size_t len = iov[i].iov_len;
    res = _TtyRead(&_vga_tty, input, len);
    if (res < 0) break;
    redd += res;
    if (redd != len) return redd;
  }
  if (!redd) return res;
  return redd;
}

#endif /* __x86_64__ */
