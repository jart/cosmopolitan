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
#include "libc/errno.h"
#include "libc/sysv/errno.h"

__privileged int __errno_xnu2linux(int e) {
  if (e < 82)
    return __errno_bsd2linux(e);
  switch (e) {

    case 82:
      // first divergence with freebsd
      // first divergence with openbsd
      return ENODEV;  // EPWROFF;
    case 83:
      return EIO;  // EDEVERR;
    case 84:
      return EOVERFLOW;
    case 85:
      return ENOEXEC;  // EBADEXEC;
    case 86:
      return ENOEXEC;  // EBADARCH;
    case 87:
      return ENOEXEC;  // ESHLIBVERS;
    case 88:
      return ENOEXEC;  // EBADMACHO;
    case 89:
      return ECANCELED;

    case 90:
      return EIDRM;
    case 91:
      return ENOMSG;
    case 92:
      return EILSEQ;
    case 93:
      return ENODATA;  // ENOATTR;
    case 94:
      return EBADMSG;
    case 95:
      return EMULTIHOP;
    case 96:
      return ENODATA;
    case 97:
      return ENOLINK;
    case 98:
      return ENOSR;
    case 99:
      return ENOSTR;

    case 100:
      return EPROTO;
    case 101:
      return ETIME;
    case 102:
      return EOPNOTSUPP;
    case 103:
      return ENOENT;  // ENOPOLICY;
    case 104:
      return ENOTRECOVERABLE;
    case 105:
      return EOWNERDEAD;
    case 106:
      return ENOBUFS;  // EQFULL;

    default:
      return e;
  }
}
