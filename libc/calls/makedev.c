/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/makedev.h"
#include "libc/dce.h"

/**
 * Creates device id.
 *
 * @param major identifies device driver
 * @param major identifies instance of device
 * @return your `st_dev` value
 * @see major(), minor()
 */
uint64_t makedev(uint32_t major, uint32_t minor) {
  if (IsWindows()) {
    return (uint64_t)major << 32 | minor;
  } else if (IsXnu()) {
    return major << 24 | minor;
  } else if (IsNetbsd()) {
    return ((major << 8) & 0x000fff00) | ((minor << 12) & 0xfff00000) |
           (minor & 0x000000ff);
  } else if (IsOpenbsd()) {
    return (major & 0xff) << 8 | (minor & 0xff) | (minor & 0xffff00) << 8;
  } else if (IsFreebsd()) {
    return (uint64_t)(major & 0xffffff00) << 32 | (major & 0x000000ff) << 8 |
           (minor & 0x0000ff00) << 24 | (minor & 0xffff00ff);
  } else {
    return (uint64_t)(major & 0xfffff000) << 32 | (major & 0x00000fff) << 8 |
           (minor & 0xffffff00) << 12 | (minor & 0x000000ff);
  }
}
