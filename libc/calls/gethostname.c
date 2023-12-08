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
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/computernameformat.h"
#include "libc/sysv/errfuns.h"

#define KERN_HOSTNAME 10

/**
 * Returns name of current host.
 *
 * For example, if the fully-qualified hostname is "host.domain.example"
 * then this SHOULD return "host" however that might not be the case; it
 * depends on how the host machine is configured. It's fair to say if it
 * has a dot, it's a FQDN, otherwise it's a node.
 *
 * The nul / mutation semantics are tricky. Here is some safe copypasta:
 *
 *     char host[254];
 *     if (gethostname(host, sizeof(host))) {
 *       strcpy(host, "localhost");
 *     }
 *
 * On Linux this is the same as `/proc/sys/kernel/hostname`.
 *
 * @param name receives output name, which is guaranteed to be complete
 *     and have a nul-terminator if this function return zero
 * @param len is size of `name` consider using `DNS_NAME_MAX + 1` (254)
 * @raise EINVAL if `len` is negative
 * @raise EFAULT if `name` is an invalid address
 * @raise ENAMETOOLONG if the underlying system call succeeded, but the
 *     returned hostname had a length equal to or greater than `len` in
 *     which case this error is raised and the buffer is modified, with
 *     as many bytes of hostname as possible excluding a nul-terminator
 * @return 0 on success, or -1 w/ errno
 */
int gethostname(char *name, size_t len) {
  int rc;
  if (len < 0) {
    rc = einval();
  } else if (!len) {
    rc = 0;
  } else if (!name) {
    rc = efault();
  } else if (IsLinux()) {
    rc = gethostname_linux(name, len);
  } else if (IsBsd()) {
    rc = gethostname_bsd(name, len, KERN_HOSTNAME);
  } else if (IsWindows()) {
    rc = gethostname_nt(name, len, kNtComputerNamePhysicalDnsHostname);
  } else {
    rc = enosys();
  }
  STRACE("gethostname([%#.*s], %'zu) → %d% m", (int)len, name, len, rc);
  return rc;
}
