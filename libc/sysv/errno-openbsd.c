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

// Notes on OpenBSD error codes
//
// - EPROCLIM is mentioned in intro(2) but no system calls are
//   documented as returning this code, and the kernel source code
//   doesn't appear to ever return this code. If it was in play, then
//   its most logical mapping would be EAGAIN.
//
// - EBADRPC isn't documented on any particular system call, but it is
//   referenced by the nfs kernel source code.
//
// - ERPCMISMATCH might possibly be raised by system calls using nfs.
//
// - EPROGUNAVAIL might possibly be raised by system calls using nfs,
//   possibly if the remote end doesn't support nfs.
//
// - EPROGMISMATCH might possibly be raised by system calls using nfs,
//   possibly if the remote end has the wrong nfs version.
//
// - EAUTH probably can't be returned by the kernel, although the nfs
//   source code raises EACCES in a place that has a comment saying it
//   should be EAUTH instead.
//
// - ENEEDAUTH isn't referenced in the kernel source code. It might only
//   be used by nfs code in userspace.
//
// - ENOATTR is raised by extattr related system calls.
//
// - EIPSEC is an ipsec processing failure, which we map to EPROTO since
//   it's a protocol error.
//

__privileged int __errno_openbsd2linux(int e) {
  if (e < 82)
    return __errno_bsd2linux(e);
  switch (e) {

    case 82:
      // first divergence with xnu
      // first divergence with freebsd
      return EPROTO;  // EIPSEC;
    case 83:
      return ENODATA;  // ENOATTR;
    case 84:
      return EILSEQ;
    case 85:
      return ENOMEDIUM;
    case 86:
      return EMEDIUMTYPE;
    case 87:
      return EOVERFLOW;
    case 88:
      return ECANCELED;
    case 89:
      return EIDRM;

    case 90:
      return ENOMSG;
    case 91:
      return ENOTSUP;
    case 92:
      return EBADMSG;
    case 93:
      return ENOTRECOVERABLE;
    case 94:
      return EOWNERDEAD;
    case 95:
      return EPROTO;

    default:
      return e;
  }
}
