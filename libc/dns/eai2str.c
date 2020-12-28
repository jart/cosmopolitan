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
#include "libc/sysv/consts/eai.h"

/**
 * Turns getaddrinfo() return code into string.
 */
const char *eai2str(int code) {
  if (code == EAI_ADDRFAMILY) return "ADDRFAMILY";
  if (code == EAI_AGAIN) return "AGAIN";
  if (code == EAI_ALLDONE) return "ALLDONE";
  if (code == EAI_BADFLAGS) return "BADFLAGS";
  if (code == EAI_CANCELED) return "CANCELED";
  if (code == EAI_FAIL) return "FAIL";
  if (code == EAI_FAMILY) return "FAMILY";
  if (code == EAI_IDN_ENCODE) return "ENCODE";
  if (code == EAI_INPROGRESS) return "INPROGRESS";
  if (code == EAI_INTR) return "INTR";
  if (code == EAI_MEMORY) return "MEMORY";
  if (code == EAI_NODATA) return "NODATA";
  if (code == EAI_NONAME) return "NONAME";
  if (code == EAI_NOTCANCELED) return "NOTCANCELED";
  if (code == EAI_OVERFLOW) return "OVERFLOW";
  if (code == EAI_SERVICE) return "SERVICE";
  if (code == EAI_SOCKTYPE) return "SOCKTYPE";
  if (code == EAI_SUCCESS) return "SUCCESS";
  if (code == EAI_SYSTEM) return "SYSTEM";
  return "???";
}
