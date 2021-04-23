/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"

const struct Error {
  const long *x;
  const char *s;
} kErrors[] = {
    {&ENOSYS, "ENOSYS"},
    {&EPERM, "EPERM"},
    {&ENOENT, "ENOENT"},
    {&ESRCH, "ESRCH"},
    {&EINTR, "EINTR"},
    {&EIO, "EIO"},
    {&ENXIO, "ENXIO"},
    {&E2BIG, "E2BIG"},
    {&ENOEXEC, "ENOEXEC"},
    {&EBADF, "EBADF"},
    {&ECHILD, "ECHILD"},
    {&EAGAIN, "EAGAIN"},
    {&ENOMEM, "ENOMEM"},
    {&EACCES, "EACCES"},
    {&EFAULT, "EFAULT"},
    {&ENOTBLK, "ENOTBLK"},
    {&EBUSY, "EBUSY"},
    {&EEXIST, "EEXIST"},
    {&EXDEV, "EXDEV"},
    {&ENODEV, "ENODEV"},
    {&ENOTDIR, "ENOTDIR"},
    {&EISDIR, "EISDIR"},
    {&EINVAL, "EINVAL"},
    {&ENFILE, "ENFILE"},
    {&EMFILE, "EMFILE"},
    {&ENOTTY, "ENOTTY"},
    {&ETXTBSY, "ETXTBSY"},
    {&EFBIG, "EFBIG"},
    {&ENOSPC, "ENOSPC"},
    {&EDQUOT, "EDQUOT"},
    {&ESPIPE, "ESPIPE"},
    {&EROFS, "EROFS"},
    {&EMLINK, "EMLINK"},
    {&EPIPE, "EPIPE"},
    {&EDOM, "EDOM"},
    {&ERANGE, "ERANGE"},
    {&EDEADLK, "EDEADLK"},
    {&ENAMETOOLONG, "ENAMETOOLONG"},
    {&ENOLCK, "ENOLCK"},
    {&ENOTEMPTY, "ENOTEMPTY"},
    {&ELOOP, "ELOOP"},
    {&ENOMSG, "ENOMSG"},
    {&EIDRM, "EIDRM"},
    {&ETIME, "ETIME"},
    {&EPROTO, "EPROTO"},
    {&EOVERFLOW, "EOVERFLOW"},
    {&EILSEQ, "EILSEQ"},
    {&EUSERS, "EUSERS"},
    {&ENOTSOCK, "ENOTSOCK"},
    {&EDESTADDRREQ, "EDESTADDRREQ"},
    {&EMSGSIZE, "EMSGSIZE"},
    {&EPROTOTYPE, "EPROTOTYPE"},
    {&ENOPROTOOPT, "ENOPROTOOPT"},
    {&EPROTONOSUPPORT, "EPROTONOSUPPORT"},
    {&ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT"},
    {&ENOTSUP, "ENOTSUP"},
    {&EOPNOTSUPP, "EOPNOTSUPP"},
    {&EPFNOSUPPORT, "EPFNOSUPPORT"},
    {&EAFNOSUPPORT, "EAFNOSUPPORT"},
    {&EADDRINUSE, "EADDRINUSE"},
    {&EADDRNOTAVAIL, "EADDRNOTAVAIL"},
    {&ENETDOWN, "ENETDOWN"},
    {&ENETUNREACH, "ENETUNREACH"},
    {&ENETRESET, "ENETRESET"},
    {&ECONNABORTED, "ECONNABORTED"},
    {&ECONNRESET, "ECONNRESET"},
    {&ENOBUFS, "ENOBUFS"},
    {&EISCONN, "EISCONN"},
    {&ENOTCONN, "ENOTCONN"},
    {&ESHUTDOWN, "ESHUTDOWN"},
    {&ETOOMANYREFS, "ETOOMANYREFS"},
    {&ETIMEDOUT, "ETIMEDOUT"},
    {&ECONNREFUSED, "ECONNREFUSED"},
    {&EHOSTDOWN, "EHOSTDOWN"},
    {&EHOSTUNREACH, "EHOSTUNREACH"},
    {&EALREADY, "EALREADY"},
    {&EINPROGRESS, "EINPROGRESS"},
    {&ESTALE, "ESTALE"},
    {&EREMOTE, "EREMOTE"},
    {&EBADMSG, "EBADMSG"},
    {&ECANCELED, "ECANCELED"},
    {&EOWNERDEAD, "EOWNERDEAD"},
    {&ENOTRECOVERABLE, "ENOTRECOVERABLE"},
    {&ENONET, "ENONET"},
    {&ERESTART, "ERESTART"},
    {&ECHRNG, "ECHRNG"},
    {&EL2NSYNC, "EL2NSYNC"},
    {&EL3HLT, "EL3HLT"},
    {&EL3RST, "EL3RST"},
    {&ELNRNG, "ELNRNG"},
    {&EUNATCH, "EUNATCH"},
    {&ENOCSI, "ENOCSI"},
    {&EL2HLT, "EL2HLT"},
    {&EBADE, "EBADE"},
    {&EBADR, "EBADR"},
    {&EXFULL, "EXFULL"},
    {&ENOANO, "ENOANO"},
    {&EBADRQC, "EBADRQC"},
    {&EBADSLT, "EBADSLT"},
    {&ENOSTR, "ENOSTR"},
    {&ENODATA, "ENODATA"},
    {&ENOSR, "ENOSR"},
    {&ENOPKG, "ENOPKG"},
    {&ENOLINK, "ENOLINK"},
    {&EADV, "EADV"},
    {&ESRMNT, "ESRMNT"},
    {&ECOMM, "ECOMM"},
    {&EMULTIHOP, "EMULTIHOP"},
    {&EDOTDOT, "EDOTDOT"},
    {&ENOTUNIQ, "ENOTUNIQ"},
    {&EBADFD, "EBADFD"},
    {&EREMCHG, "EREMCHG"},
    {&ELIBACC, "ELIBACC"},
    {&ELIBBAD, "ELIBBAD"},
    {&ELIBSCN, "ELIBSCN"},
    {&ELIBMAX, "ELIBMAX"},
    {&ELIBEXEC, "ELIBEXEC"},
    {&ESTRPIPE, "ESTRPIPE"},
    {&EUCLEAN, "EUCLEAN"},
    {&ENOTNAM, "ENOTNAM"},
    {&ENAVAIL, "ENAVAIL"},
    {&EISNAM, "EISNAM"},
    {&EREMOTEIO, "EREMOTEIO"},
    {&ENOMEDIUM, "ENOMEDIUM"},
    {&EMEDIUMTYPE, "EMEDIUMTYPE"},
    {&ENOKEY, "ENOKEY"},
    {&EKEYEXPIRED, "EKEYEXPIRED"},
    {&EKEYREVOKED, "EKEYREVOKED"},
    {&EKEYREJECTED, "EKEYREJECTED"},
    {&ERFKILL, "ERFKILL"},
    {&EHWPOISON, "EHWPOISON"},
};

static const char *geterrname(long x) {
  int i;
  if (x) {
    for (i = 0; i < ARRAYLEN(kErrors); ++i) {
      if (x == *kErrors[i].x) {
        return kErrors[i].s;
      }
    }
  }
  return "EUNKNOWN";
}

/**
 * Converts errno value to string.
 * @return 0 on success, or error code
 */
int strerror_r(int err, char *buf, size_t size) {
  char *p;
  const char *s;
  err &= 0xFFFF;
  s = geterrname(err);
  p = buf;
  if (strlen(s) + 1 + 5 + 1 + 1 <= size) {
    p = stpcpy(p, s);
    *p++ = '[';
    p += uint64toarray_radix10(err, p);
    *p++ = ']';
  }
  if (p - buf < size) {
    *p++ = '\0';
  }
  return 0;
}
