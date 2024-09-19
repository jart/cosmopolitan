/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/describeflags.h"
#include "libc/macros.h"
#include "libc/sysv/consts/msg.h"

const char *_DescribeMsg(char buf[16], int x) {
  const struct DescribeFlags kMsgFlags[] = {
      {MSG_FASTOPEN, "FASTOPEN"},    // order matters
      {MSG_OOB, "OOB"},              //
      {MSG_PEEK, "PEEK"},            //
      {MSG_DONTROUTE, "DONTROUTE"},  //
      {MSG_DONTWAIT, "DONTWAIT"},    //
      {MSG_WAITALL, "WAITALL"},      //
      {MSG_NOSIGNAL, "NOSIGNAL"},    //
      {MSG_TRUNC, "TRUNC"},          //
      {MSG_CTRUNC, "CTRUNC"},        //
  };
  return _DescribeFlags(buf, 16, kMsgFlags, ARRAYLEN(kMsgFlags), "MSG_", x);
}
