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
#include "libc/dce.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"

#define VERSION 0x0202 /* Windows Vista+ */

/**
 * Information about underyling Windows Sockets implemmentation.
 *
 * Cosmopolitan automatically calls YOINK() on this symbol when its
 * Berkeley Socket wrappers are linked. The latest version of Winsock
 * was introduced alongside x64, so this should never fail.
 */
hidden struct NtWsaData kNtWsaData;

textwindows static void winsockfini(void) {
  WSACleanup();
}

textwindows void winsockinit(void) {
  int rc;
  atexit(winsockfini);
  if ((rc = WSAStartup(VERSION, &kNtWsaData)) != 0 ||
      kNtWsaData.wVersion != VERSION) {
    abort();
  }
}
