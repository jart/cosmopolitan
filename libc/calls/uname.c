/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/utsname-linux.internal.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/computernameformat.h"
#include "libc/nt/struct/teb.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define CTL_KERN        1
#define KERN_OSTYPE     1
#define KERN_OSRELEASE  2
#define KERN_VERSION    4
#define KERN_HOSTNAME   10
#define KERN_DOMAINNAME 22

#define CTL_HW     6
#define HW_MACHINE 1

#define COSMOPOLITAN_VERSION_STR__(x, y, z) #x "." #y "." #z
#define COSMOPOLITAN_VERSION_STR_(x, y, z)  COSMOPOLITAN_VERSION_STR__(x, y, z)
#define COSMOPOLITAN_VERSION_STR                                            \
  COSMOPOLITAN_VERSION_STR_(__COSMOPOLITAN_MAJOR__, __COSMOPOLITAN_MINOR__, \
                            __COSMOPOLITAN_PATCH__)

// Gets BSD sysctl() string, guaranteeing NUL-terminator.
// We ignore errors since this syscall mutates on ENOMEM.
// In that case, sysctl() doesn't guarantee the nul term.
static void GetBsdStr(int c0, int c1, char *s) {
  char *p;
  int e = errno;
  size_t n = SYS_NMLN;
  int cmd[2] = {c0, c1};
  bzero(s, n), --n;
  sys_sysctl(cmd, 2, s, &n, NULL, 0);
  errno = e;
  // sysctl kern.version is too verbose for uname
  if ((p = strchr(s, '\n'))) {
    *p = 0;
  }
}

// Gets NT name ignoring errors with guaranteed NUL-terminator.
static textwindows void GetNtName(char *name, int kind) {
  char16_t name16[256];
  uint32_t size = ARRAYLEN(name16);
  if (GetComputerNameEx(kind, name16, &size)) {
    tprecode16to8(name, SYS_NMLN, name16);
  } else {
    name[0] = 0;
  }
}

static inline textwindows int GetNtMajorVersion(void) {
#ifdef __x86_64__
  return NtGetPeb()->OSMajorVersion;
#else
  return 0;
#endif
}

static inline textwindows int GetNtMinorVersion(void) {
#ifdef __x86_64__
  return NtGetPeb()->OSMinorVersion;
#else
  return 0;
#endif
}

static inline textwindows int GetNtBuildNumber(void) {
#ifdef __x86_64__
  return NtGetPeb()->OSBuildNumber;
#else
  return 0;
#endif
}

static textwindows void GetNtVersion(char *p) {
  p = FormatUint32(p, GetNtMajorVersion()), *p++ = '.';
  p = FormatUint32(p, GetNtMinorVersion()), *p++ = '-';
  p = FormatUint32(p, GetNtBuildNumber());
}

static const char *Str(int rc, const char *s) {
  return !rc ? s : "n/a";
}

/**
 * Asks kernel to give us `uname -a` data.
 *
 * - `machine` should be one of:
 *   - `x86_64`
 *   - `amd64`
 *
 * - `sysname` should be one of:
 *   - `Linux`
 *   - `FreeBSD`
 *   - `NetBSD`
 *   - `OpenBSD`
 *   - `Darwin`
 *   - `Windows`
 *
 * - `version` contains the distro name, version, and release date. On
 *   FreeBSD/NetBSD/OpenBSD this may contain newline characters, which
 *   this polyfill hides by setting the first newline character to nul
 *   although the information can be restored by putting newline back.
 *   BSDs usually repeat the `sysname` and `release` in the `version`.
 *
 * - `nodename` *may* be the first label of the fully qualified hostname
 *   of the current host. This is equivalent to gethostname(), except we
 *   guarantee a NUL-terminator here with truncation, which should never
 *   happen unless the machine violates the DNS standard. If it has dots
 *   then it's fair to assume it's an FQDN. On Linux this is the same as
 *   the content of `/proc/sys/kernel/hostname`.
 *
 * - `domainname` *may* be the higher-order labels of the FQDN for this
 *   host. This is equivalent to getdomainname(), except we guarantee a
 *   NUL-terminator here with truncation, which should never happen w/o
 *   violating the DNS standard. If this field is not present, it'll be
 *   coerced to empty string. On Linux, this is the same as the content
 *   of `/proc/sys/kernel/domainname`.
 *
 * The returned fields are guaranteed to be nul-terminated.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if `buf` isn't valid
 * @raise ENOSYS on metal
 */
int uname(struct utsname *uts) {
  int rc;
  if (!uts || (IsAsan() && !__asan_is_valid(uts, sizeof(*uts)))) {
    rc = efault();
  } else if (IsLinux()) {
    struct utsname_linux linux;
    if (!(rc = sys_uname_linux(&linux))) {
      strlcpy(uts->sysname, linux.sysname, SYS_NMLN);
      strlcpy(uts->nodename, linux.nodename, SYS_NMLN);
      strlcpy(uts->release, linux.release, SYS_NMLN);
      strlcpy(uts->version, linux.version, SYS_NMLN);
      strlcpy(uts->machine, linux.machine, SYS_NMLN);
      strlcpy(uts->domainname, linux.domainname, SYS_NMLN);
      if (!strcmp(uts->domainname, "(none)")) {
        uts->domainname[0] = 0;
      }
    }
  } else if (IsBsd()) {
    GetBsdStr(CTL_KERN, KERN_OSTYPE, uts->sysname);
    GetBsdStr(CTL_KERN, KERN_HOSTNAME, uts->nodename);
    GetBsdStr(CTL_KERN, KERN_DOMAINNAME, uts->domainname);
    GetBsdStr(CTL_KERN, KERN_OSRELEASE, uts->release);
    GetBsdStr(CTL_KERN, KERN_VERSION, uts->version);
    GetBsdStr(CTL_HW, HW_MACHINE, uts->machine);
    rc = 0;
  } else if (IsWindows()) {
    stpcpy(uts->sysname, "Windows");
    stpcpy(uts->machine, "x86_64");
    GetNtVersion(uts->release);
    GetNtName(uts->nodename, kNtComputerNamePhysicalDnsHostname);
    GetNtName(uts->domainname, kNtComputerNamePhysicalDnsDomain);
    rc = 0;
  } else {
    rc = enosys();
  }
  if (!rc) {
    char buf[SYS_NMLN];
    stpcpy(buf, "Cosmopolitan " COSMOPOLITAN_VERSION_STR);
    if (*MODE) {
      strlcat(buf, " MODE=" MODE, SYS_NMLN);
    }
    if (*uts->version) {
      strlcat(buf, "; ", SYS_NMLN);
      strlcat(buf, uts->version, SYS_NMLN);
    }
    strlcpy(uts->version, buf, SYS_NMLN);
  }
  STRACE("uname([{%#s, %#s, %#s, %#s, %#s, %#s}]) → %d% m",
         Str(rc, uts->sysname), Str(rc, uts->nodename), Str(rc, uts->release),
         Str(rc, uts->version), Str(rc, uts->machine), Str(rc, uts->domainname),
         rc);
  return rc;
}
