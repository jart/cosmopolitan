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
 * Converts errno value to descriptive sentence.
 * @return non-null rodata string or null if not found
 */
const char* _strerdoc(int e) {
  switch (e) {
    case EPERM:
      return "Operation not permitted";
    case ENOENT:
      return "No such file or directory";
    case ESRCH:
      return "No such process";
    case EINTR:
      return "The greatest of all errnos";
    case EIO:
      return "I/O error";
    case ENXIO:
      return "No such device or address";
    case E2BIG:
      return "Arg list too long";
    case ENOEXEC:
      return "Exec format error";
    case EBADF:
      return "Bad file descriptor";
    case ECHILD:
      return "No child process";
    case EAGAIN:
      return "Resource temporarily unavailable";
    case ENOMEM:
      return "Out of memory";
    case EACCES:
      return "Permission denied";
    case EFAULT:
      return "Invalid memory address";
    case ENOTBLK:
      return "Block device required";
    case EBUSY:
      return "Device or resource busy";
    case EEXIST:
      return "File exists";
    case EXDEV:
      return "Improper link";
    case ENODEV:
      return "No such device";
    case ENOTDIR:
      return "Not a directory";
    case EISDIR:
      return "Is a directory";
    case EINVAL:
      return "Invalid argument";
    case ENFILE:
      return "Too many open files in system";
    case EMFILE:
      return "Too many open files";
    case ENOTTY:
      return "Inappropriate i/o control operation";
    case ETXTBSY:
      return "Text file busy";
    case EFBIG:
      return "File too large";
    case ENOSPC:
      return "No space left on device";
    case ESPIPE:
      return "Invalid seek";
    case EROFS:
      return "Read-only filesystem";
    case EMLINK:
      return "Too many links";
    case EPIPE:
      return "Broken pipe";
    case EDOM:
      return "Argument outside function domain";
    case ERANGE:
      return "Result too large";
    case EDEADLK:
      return "Resource deadlock avoided";
    case ENAMETOOLONG:
      return "Filename too long";
    case ENOLCK:
      return "No locks available";
    case ENOSYS:
      return "Function not implemented";
    case ENOTEMPTY:
      return "Directory not empty";
    case ELOOP:
      return "Too many levels of symbolic links";
    case ENOMSG:
      return "No message error";
    case EIDRM:
      return "Identifier removed";
    case ENOSTR:
      return "No string";
    case ENODATA:
      return "No data";
    case ETIME:
      return "Timer expired";
    case ENOSR:
      return "Out of streams resources";
    case ENONET:
      return "No network";
    case EREMOTE:
      return "Remote error";
    case ENOLINK:
      return "Link severed";
    case EPROTO:
      return "Protocol error";
    case EMULTIHOP:
      return "Multihop attempted";
    case EBADMSG:
      return "Bad message";
    case EOVERFLOW:
      return "Overflow error";
    case EBADFD:
      return "File descriptor in bad state";
    case EFTYPE:
      return "Inappropriate file type or format";
    case EILSEQ:
      return "Unicode decoding error";
    case ERESTART:
      return "Please restart syscall";
    case EUSERS:
      return "Too many users";
    case ENOTSOCK:
      return "Not a socket";
    case EDESTADDRREQ:
      return "Destination address required";
    case EMSGSIZE:
      return "Message too long";
    case EPROTOTYPE:
      return "Protocol wrong type for socket";
    case ENOPROTOOPT:
      return "Protocol not available";
    case EPROTONOSUPPORT:
      return "Protocol not supported";
    case ESOCKTNOSUPPORT:
      return "Socket type not supported";
    case ENOTSUP:  // a.k.a. EOPNOTSUPP
      return "Not supported";
    case EPFNOSUPPORT:
      return "Protocol family not supported";
    case EAFNOSUPPORT:
      return "Address family not supported";
    case EADDRINUSE:
      return "Address already in use";
    case EADDRNOTAVAIL:
      return "Address not available";
    case ENETDOWN:
      return "Network is down";
    case ENETUNREACH:
      return "Host is unreachable";
    case ENETRESET:
      return "Connection reset by network";
    case ECONNABORTED:
      return "Connection reset before accept";
    case ECONNRESET:
      return "Connection reset by client";
    case ENOBUFS:
      return "No buffer space available";
    case EISCONN:
      return "Socket is connected";
    case ENOTCONN:
      return "Transport endpoint is not connected";
    case ESHUTDOWN:
      return "Transport endpoint shutdown";
    case ETOOMANYREFS:
      return "Too many references: cannot splice";
    case ETIMEDOUT:
      return "Connection timed out";
    case ECONNREFUSED:
      return "Connection refused error";
    case EHOSTDOWN:
      return "Host down error";
    case EHOSTUNREACH:
      return "Host unreachable error";
    case EALREADY:
      return "Connection already in progress";
    case EINPROGRESS:
      return "Operation already in progress";
    case ESTALE:
      return "Stale error";
    case EDQUOT:
      return "Disk quota exceeded";
    case ENOMEDIUM:
      return "No medium found";
    case EMEDIUMTYPE:
      return "Wrong medium type";
    case ECANCELED:
      return "Operation canceled";
    case EOWNERDEAD:
      return "Owner died";
    case ENOTRECOVERABLE:
      return "State not recoverable";
    case ERFKILL:
      return "Operation not possible due to RF-kill";
    case EHWPOISON:
      return "Memory page has hardware error";
    default:
      return 0;
  }
}
