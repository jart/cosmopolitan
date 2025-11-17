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
 * Identifies device driver.
 *
 *     struct stat st;
 *     stat("/", &st);
 *     printf("%u,%u\n", major(st.st_dev), minor(st.st_dev));
 *
 * @param dev is `st_dev` field returned by stat(), fstatat(), etc.
 * @see minor(), makedev()
 */
uint32_t major(uint64_t dev) {
  if (IsWindows()) {
    return dev >> 32;
  } else if (IsXnu()) {
    return (dev >> 24) & 0xff;
  } else if (IsNetbsd()) {
    return (dev & 0x000fff00) >> 8;
  } else if (IsOpenbsd()) {
    return (dev >> 8) & 0xff;
  } else if (IsFreebsd()) {
    return ((dev >> 32) & 0xffffff00) | ((dev >> 8) & 0x000000ff);
  } else {
    return ((dev >> 32) & 0xfffff000) | ((dev >> 8) & 0x00000fff);
  }
}
