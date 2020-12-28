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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nt/nt/process.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processbasicinformation.h"

static textwindows noinline int32_t getppid$nt(void) {
  struct NtProcessBasicInformation ProcessInformation;
  uint32_t gotsize = 0;
  if (!NtError(
          NtQueryInformationProcess(GetCurrentProcess(), 0, &ProcessInformation,
                                    sizeof(ProcessInformation), &gotsize)) &&
      gotsize >= sizeof(ProcessInformation) &&
      ProcessInformation.InheritedFromUniqueProcessId) {
    /* TODO(jart): Fix type mismatch and do we need to close this? */
    return ProcessInformation.InheritedFromUniqueProcessId;
  }
  return GetCurrentProcessId();
}

/**
 * Returns parent process id.
 * @asyncsignalsafe
 */
int32_t getppid(void) {
  if (!IsWindows()) {
    return getppid$sysv();
  } else {
    return getppid$nt();
  }
}
