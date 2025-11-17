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
#include "libc/fmt/magnumstrs.internal.h"

/**
 * Converts errno value to symbolic name.
 * @return non-null rodata string or null if not found
 */
__privileged const char* _strerrno(int e) {
  switch (e) {
    case EPERM:
      return "EPERM";
    case ENOENT:
      return "ENOENT";
    case ESRCH:
      return "ESRCH";
    case EINTR:
      return "EINTR";
    case EIO:
      return "EIO";
    case ENXIO:
      return "ENXIO";
    case E2BIG:
      return "E2BIG";
    case ENOEXEC:
      return "ENOEXEC";
    case EBADF:
      return "EBADF";
    case ECHILD:
      return "ECHILD";
    case EAGAIN:
      return "EAGAIN";
    case ENOMEM:
      return "ENOMEM";
    case EACCES:
      return "EACCES";
    case EFAULT:
      return "EFAULT";
    case ENOTBLK:
      return "ENOTBLK";
    case EBUSY:
      return "EBUSY";
    case EEXIST:
      return "EEXIST";
    case EXDEV:
      return "EXDEV";
    case ENODEV:
      return "ENODEV";
    case ENOTDIR:
      return "ENOTDIR";
    case EISDIR:
      return "EISDIR";
    case EINVAL:
      return "EINVAL";
    case ENFILE:
      return "ENFILE";
    case EMFILE:
      return "EMFILE";
    case ENOTTY:
      return "ENOTTY";
    case ETXTBSY:
      return "ETXTBSY";
    case EFBIG:
      return "EFBIG";
    case ENOSPC:
      return "ENOSPC";
    case ESPIPE:
      return "ESPIPE";
    case EROFS:
      return "EROFS";
    case EMLINK:
      return "EMLINK";
    case EPIPE:
      return "EPIPE";
    case EDOM:
      return "EDOM";
    case ERANGE:
      return "ERANGE";
    case EDEADLK:
      return "EDEADLK";
    case ENAMETOOLONG:
      return "ENAMETOOLONG";
    case ENOLCK:
      return "ENOLCK";
    case ENOSYS:
      return "ENOSYS";
    case ENOTEMPTY:
      return "ENOTEMPTY";
    case ELOOP:
      return "ELOOP";
    case ENOMSG:
      return "ENOMSG";
    case EIDRM:
      return "EIDRM";
    case ENOSTR:
      return "ENOSTR";
    case ENODATA:
      return "ENODATA";
    case ETIME:
      return "ETIME";
    case ENOSR:
      return "ENOSR";
    case ENONET:
      return "ENONET";
    case EREMOTE:
      return "EREMOTE";
    case ENOLINK:
      return "ENOLINK";
    case EPROTO:
      return "EPROTO";
    case EMULTIHOP:
      return "EMULTIHOP";
    case EBADMSG:
      return "EBADMSG";
    case EOVERFLOW:
      return "EOVERFLOW";
    case EBADFD:
      return "EBADFD";
    case EFTYPE:
      return "EFTYPE";
    case EILSEQ:
      return "EILSEQ";
    case ERESTART:
      return "ERESTART";
    case EUSERS:
      return "EUSERS";
    case ENOTSOCK:
      return "ENOTSOCK";
    case EDESTADDRREQ:
      return "EDESTADDRREQ";
    case EMSGSIZE:
      return "EMSGSIZE";
    case EPROTOTYPE:
      return "EPROTOTYPE";
    case ENOPROTOOPT:
      return "ENOPROTOOPT";
    case EPROTONOSUPPORT:
      return "EPROTONOSUPPORT";
    case ESOCKTNOSUPPORT:
      return "ESOCKTNOSUPPORT";
    case ENOTSUP:  // a.k.a. EOPNOTSUPP
      return "ENOTSUP";
    case EPFNOSUPPORT:
      return "EPFNOSUPPORT";
    case EAFNOSUPPORT:
      return "EAFNOSUPPORT";
    case EADDRINUSE:
      return "EADDRINUSE";
    case EADDRNOTAVAIL:
      return "EADDRNOTAVAIL";
    case ENETDOWN:
      return "ENETDOWN";
    case ENETUNREACH:
      return "ENETUNREACH";
    case ENETRESET:
      return "ENETRESET";
    case ECONNABORTED:
      return "ECONNABORTED";
    case ECONNRESET:
      return "ECONNRESET";
    case ENOBUFS:
      return "ENOBUFS";
    case EISCONN:
      return "EISCONN";
    case ENOTCONN:
      return "ENOTCONN";
    case ESHUTDOWN:
      return "ESHUTDOWN";
    case ETOOMANYREFS:
      return "ETOOMANYREFS";
    case ETIMEDOUT:
      return "ETIMEDOUT";
    case ECONNREFUSED:
      return "ECONNREFUSED";
    case EHOSTDOWN:
      return "EHOSTDOWN";
    case EHOSTUNREACH:
      return "EHOSTUNREACH";
    case EALREADY:
      return "EALREADY";
    case EINPROGRESS:
      return "EINPROGRESS";
    case ESTALE:
      return "ESTALE";
    case EDQUOT:
      return "EDQUOT";
    case ENOMEDIUM:
      return "ENOMEDIUM";
    case EMEDIUMTYPE:
      return "EMEDIUMTYPE";
    case ECANCELED:
      return "ECANCELED";
    case EOWNERDEAD:
      return "EOWNERDEAD";
    case ENOTRECOVERABLE:
      return "ENOTRECOVERABLE";
    case ERFKILL:
      return "ERFKILL";
    case EHWPOISON:
      return "EHWPOISON";
    default:
      return 0;
  }
}
