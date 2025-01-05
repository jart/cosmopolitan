/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/nt/process.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processbasicinformation.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"

int sys_getppid_nt_win32;
int sys_getppid_nt_cosmo;

textwindows static int sys_getppid_nt_ntdll(void) {
  struct NtProcessBasicInformation ProcessInformation;
  uint32_t gotsize = 0;
  if (!NtError(
          NtQueryInformationProcess(GetCurrentProcess(), 0, &ProcessInformation,
                                    sizeof(ProcessInformation), &gotsize)) &&
      gotsize >= sizeof(ProcessInformation) &&
      ProcessInformation.InheritedFromUniqueProcessId) {
    return ProcessInformation.InheritedFromUniqueProcessId;
  }
  return 0;
}

static void sys_getppid_nt_extract(const char *str) {
  int c;
  int win32 = 0;
  int cosmo = 0;
  if (str) {
    for (;;) {
      c = *str;
      if (!('0' <= c && c <= '9'))
        break;
      win32 *= 10;
      win32 += c - '0';
      ++str;
    }
    if (win32 && *str++ == ':') {
      for (;;) {
        c = *str;
        if (!('0' <= c && c <= '9'))
          break;
        cosmo *= 10;
        cosmo += c - '0';
        ++str;
      }
      if (win32 == sys_getppid_nt_ntdll()) {
        sys_getppid_nt_win32 = win32;
        sys_getppid_nt_cosmo = cosmo;
      }
    }
  }
}

__attribute__((__constructor__(90))) static void init(void) {
  if (!IsWindows())
    return;
  sys_getppid_nt_extract(getenv("_COSMO_PPID"));
}

textwindows int sys_getppid_nt(void) {
  if (sys_getppid_nt_cosmo)
    return sys_getppid_nt_cosmo;
  return sys_getppid_nt_ntdll();
}

textwindows void sys_getppid_nt_wipe(int win32, int cosmo) {
  sys_getppid_nt_win32 = win32;
  sys_getppid_nt_cosmo = cosmo;
}
