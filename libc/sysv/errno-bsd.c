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

__privileged int __errno_bsd2linux(int e) {
  switch (e) {

    case 1:
      return EPERM;
    case 2:
      return ENOENT;
    case 3:
      return ESRCH;
    case 4:
      return EINTR;
    case 5:
      return EIO;
    case 6:
      return ENXIO;
    case 7:
      return E2BIG;
    case 8:
      return ENOEXEC;
    case 9:
      return EBADF;

    case 10:
      return ECHILD;
    case 11:
      return EDEADLK;
    case 12:
      return ENOMEM;
    case 13:
      return EACCES;
    case 14:
      return EFAULT;
    case 15:
      return ENOTBLK;
    case 16:
      return EBUSY;
    case 17:
      return EEXIST;
    case 18:
      return EXDEV;
    case 19:
      return ENODEV;

    case 20:
      return ENOTDIR;
    case 21:
      return EISDIR;
    case 22:
      return EINVAL;
    case 23:
      return ENFILE;
    case 24:
      return EMFILE;
    case 25:
      return ENOTTY;
    case 26:
      return ETXTBSY;
    case 27:
      return EFBIG;
    case 28:
      return ENOSPC;
    case 29:
      return ESPIPE;

    case 30:
      return EROFS;
    case 31:
      return EMLINK;
    case 32:
      return EPIPE;
    case 33:
      return EDOM;
    case 34:
      return ERANGE;
    case 35:
      return EAGAIN;
    case 36:
      return EINPROGRESS;
    case 37:
      return EALREADY;
    case 38:
      return ENOTSOCK;
    case 39:
      return EDESTADDRREQ;

    case 40:
      return EMSGSIZE;
    case 41:
      return EPROTOTYPE;
    case 42:
      return ENOPROTOOPT;
    case 43:
      return EPROTONOSUPPORT;
    case 44:
      return ESOCKTNOSUPPORT;
    case 45:
      return EOPNOTSUPP;
    case 46:
      return EPFNOSUPPORT;
    case 47:
      return EAFNOSUPPORT;
    case 48:
      return EADDRINUSE;
    case 49:
      return EADDRNOTAVAIL;

    case 50:
      return ENETDOWN;
    case 51:
      return ENETUNREACH;
    case 52:
      return ENETRESET;
    case 53:
      return ECONNABORTED;
    case 54:
      return ECONNRESET;
    case 55:
      return ENOBUFS;
    case 56:
      return EISCONN;
    case 57:
      return ENOTCONN;
    case 58:
      return ESHUTDOWN;
    case 59:
      return ETOOMANYREFS;

    case 60:
      return ETIMEDOUT;
    case 61:
      return ECONNREFUSED;
    case 62:
      return ELOOP;
    case 63:
      return ENAMETOOLONG;
    case 64:
      return EHOSTDOWN;
    case 65:
      return EHOSTUNREACH;
    case 66:
      return ENOTEMPTY;
    case 67:
      return EAGAIN;  // EPROCLIM;
    case 68:
      return EUSERS;
    case 69:
      return EDQUOT;

    case 70:
      return ESTALE;
    case 71:
      return EREMOTE;
    case 72:
      return EPROTO;  // EBADRPC;
    case 73:
      return EPROTO;  // ERPCMISMATCH;
    case 74:
      return EPROTONOSUPPORT;  // EPROGUNAVAIL;
    case 75:
      return EPROTONOSUPPORT;  // EPROGMISMATCH;
    case 76:
      return ESOCKTNOSUPPORT;  // EPROCUNAVAIL;
    case 77:
      return ENOLCK;
    case 78:
      return ENOSYS;
    case 79:
      return EFTYPE;

    case 80:
      return EACCES;  // EAUTH;
    case 81:
      return EACCES;  // ENEEDAUTH;

    default:
      return e;
  }
}
