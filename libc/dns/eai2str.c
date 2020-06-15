/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
