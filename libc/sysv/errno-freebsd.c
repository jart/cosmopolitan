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

// Notes on FreeBSD error codes
//
// - ENOTCAPABLE is raised by filesystem system calls (e.g. openat,
//   unlink, rename, etc.) for path related errors. For example, FreeBSD
//   lets you put a process in a mode where it can't use absolute paths.
//   For this reason, EACCES is the most logical translation. However
//   ENOTCAPABLE may also raised by sendfile() similar to EBADF.
//
// - EINTEGRITY is raised by many file filesystem system calls when
//   corrupted data is detected while reading from the filesystem. The
//   most appropriate translation is probably EIO.
//
// - EDOOFUS is something the regents of the University of California
//   would never do, so we don't need to worry about mapping this code.
//   However the compiler generates better code if we do.
//

__privileged int __errno_freebsd2linux(int e) {
  if (e < 82)
    return __errno_bsd2linux(e);
  switch (e) {

    case 82:
      // first divergence with xnu
      // first divergence with openbsd
      return EIDRM;
    case 83:
      return ENOMSG;
    case 84:
      return EOVERFLOW;
    case 85:
      // first divergence with netbsd
      return ECANCELED;
    case 86:
      return EILSEQ;
    case 87:
      return ENODATA;  // ENOATTR;
    case 88:
      return ENOTSUP;  // EDOOFUS;
    case 89:
      return EBADMSG;

    case 90:
      return EMULTIHOP;
    case 91:
      return ENOLINK;
    case 92:
      return EPROTO;
    case 93:
      return EACCES;  // ENOTCAPABLE;
    case 94:
      return EACCES;  // ECAPMODE;
    case 95:
      return ENOTRECOVERABLE;
    case 96:
      return EOWNERDEAD;
    case 97:
      return EIO;  // EINTEGRITY;

    default:
      return e;
  }
}
