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
#include "libc/intrin/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/utsname-netbsd.internal.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/log.h"
#include "libc/nt/enum/computernameformat.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static inline textwindows noasan int NtGetMajorVersion(void) {
  return NtGetPeb()->OSMajorVersion;
}

static inline textwindows noasan int NtGetMinorVersion(void) {
  return NtGetPeb()->OSMinorVersion;
}

static inline textwindows noasan int NtGetBuildNumber(void) {
  return NtGetPeb()->OSBuildNumber;
}

/**
 * Asks kernel to give us the `uname -a` data.
 * @return 0 on success, or -1 w/ errno
 */
int uname(struct utsname *lool) {
  int rc;
  char *out, *p;
  size_t i, j, len;
  if (!lool) return efault();
  if (!lool || (IsAsan() && !__asan_is_valid(lool, sizeof(*lool)))) {
    rc = efault();
  } else {
    if (!IsWindows()) {
      if (IsLinux() || IsFreebsd()) {
        char tmp[sizeof(struct utsname)];
        bzero(tmp, sizeof(tmp));
        if ((rc = sys_uname(tmp)) != -1) {
          out = (char *)lool;
          for (i = j = 0;;) {
            len = strlen(&tmp[j]);
            if (len >= sizeof(struct utsname) - i) break;
            memcpy(&out[i], &tmp[j], len + 1);
            i += SYS_NMLN;
            j += len;
            while (j < sizeof(tmp) && tmp[j] == '\0') ++j;
            if (j == sizeof(tmp)) break;
          }
        }
      } else if (IsXnu()) {
        strcpy(lool->sysname, "XNU's Not UNIX!");
        gethostname_bsd(lool->nodename, sizeof(lool->nodename));
        rc = 0;
      } else if (IsOpenbsd()) {
        strcpy(lool->sysname, "OpenBSD");
        gethostname_bsd(lool->nodename, sizeof(lool->nodename));
        rc = 0;
      } else if (IsNetbsd()) {
        strcpy(lool->sysname, "NetBSD");
        gethostname_bsd(lool->nodename, sizeof(lool->nodename));
        rc = 0;
      } else {
        rc = enosys();
      }
    } else {
      p = lool->release;
      p = FormatUint32(p, NtGetMajorVersion()), *p++ = '.';
      p = FormatUint32(p, NtGetMinorVersion()), *p++ = '-';
      p = FormatUint32(p, NtGetBuildNumber());
      strcpy(lool->sysname, "The New Technology");
      strcpy(lool->machine, "x86_64");
      rc = gethostname_nt(lool->nodename, sizeof(lool->nodename),
                          kNtComputerNamePhysicalDnsHostname);
      rc |= gethostname_nt(lool->domainname, sizeof(lool->domainname),
                           kNtComputerNamePhysicalDnsDomain);
    }
  }
  STRACE("uname([%s, %s, %s, %s, %s, %s]) → %d% m", lool->sysname,
         lool->nodename, lool->release, lool->version, lool->machine,
         lool->domainname, rc);
  return rc;
}
