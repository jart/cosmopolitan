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
#include "libc/dns/dns.h"

/**
 * Turns getaddrinfo() return code into string.
 */
const char *gai_strerror(int code) {
  switch (code) {
    case EAI_ADDRFAMILY:
      return "ADDRFAMILY";
    case EAI_AGAIN:
      return "AGAIN";
    case EAI_ALLDONE:
      return "ALLDONE";
    case EAI_BADFLAGS:
      return "BADFLAGS";
    case EAI_CANCELED:
      return "CANCELED";
    case EAI_FAIL:
      return "FAIL";
    case EAI_FAMILY:
      return "FAMILY";
    case EAI_IDN_ENCODE:
      return "ENCODE";
    case EAI_INPROGRESS:
      return "INPROGRESS";
    case EAI_INTR:
      return "INTR";
    case EAI_MEMORY:
      return "MEMORY";
    case EAI_NODATA:
      return "NODATA";
    case EAI_NONAME:
      return "NONAME";
    case EAI_NOTCANCELED:
      return "NOTCANCELED";
    case EAI_OVERFLOW:
      return "OVERFLOW";
    case EAI_SERVICE:
      return "SERVICE";
    case EAI_SOCKTYPE:
      return "SOCKTYPE";
    case EAI_SUCCESS:
      return "SUCCESS";
    case EAI_SYSTEM:
      return "SYSTEM";
    default:
      return "???";
  }
}
