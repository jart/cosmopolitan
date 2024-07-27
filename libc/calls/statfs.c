/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/statfs-meta.internal.h"
#include "libc/calls/struct/statfs.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns information about filesystem.
 *
 * The `struct statfs` returned has the following fields:
 *
 * - `f_fstypename` holds a NUL-terminated string identifying the file
 *   system type. On Linux, this will usually be "nfs". On FreeBSD, it
 *   will usually be "zfs". On OpenBSD and NetBSD, it's usually "ffs".
 *   On MacOS it's usually "apfs", and on Windows it's usually "NTFS".
 *
 * - `f_bsize` is the optimal transfer block size. This may be used to
 *   appropriately chunk your i/o operations. On local file systems it
 *   will usually be somewhere between 4096 and 131072 bytes. With NFS
 *   it may be as high as 512kb.
 *
 * - `f_frsize` is the fragment size of the file system. This could be
 *   anywhere between 512 and 4096 bytes for local filesystems usually
 *   although it could go higher. It should less than, or equal to the
 *   `f_bsize`. This fragment size is what you want to use to multiply
 *   other fields that count blocks into a byte count.
 *
 * - `f_bfree` is the number of free blocks in the filesystem. You can
 *   multiply this number by `f_frsize` to obtain the free byte count.
 *
 * - `f_bavail` is the number of free blocks in the filesystem you can
 *   access from userspace. It's less than or equal to `f_bfree` which
 *   generally has some blocks reserved for root in a pinch. You could
 *   multiply this by `f_frsize` to convert this number to bytes.
 *
 * - `f_files` is the total number of file nodes. Not every OS has it.
 *   On Windows for instance it's currently always `INT64_MAX`. It has
 *   an unspecified meaning. It should be seen as informative.
 *
 * - `f_fsid` is an opaque data structure that uniquely identifies the
 *   filesystem. We're not yet certain how reliable this is across the
 *   various OSes and filesystem types.
 *
 * - `f_namelen` is basically the same as `NAME_MAX` which seems to be
 *   255 on all the OSes we've evaluated. It's the maximum length when
 *   it comes to individual components in a filesystem path.
 *
 * - `f_type` is an OS-specific file system type ID. This is just some
 *   magic number. No defines are provided by Cosmopolitan Libc for it
 *
 * - `f_flags` specifies the options used when a filesystem is mounted
 *   and the numbers vary across OSes. Cosmopolitan Libc polyfills the
 *   magic numbers somewhat consistently. If `IsWindows()` is set then
 *   the constants defined by Microsoft (e.g. `FILE_READ_ONLY_VOLUME`)
 *   should be used. Otherwise on any other UNIX system, the following
 *   constants are provided. You should check each constant at runtime
 *   before using them, to determine if they're non-zero, for support.
 *
 *   - `ST_RDONLY` if mounted in read-only mode (works UNIX + Windows)
 *   - `ST_NOSUID` if setuid binaries are forbidden (all UNIX support)
 *   - `ST_NODEV` when device file access forbidden (all UNIX support)
 *   - `ST_NOEXEC` when a file executions forbidden (all UNIX support)
 *   - `ST_SYNCHRONOUS`, if `O_SYNC` always happens (all UNIX support)
 *   - `ST_NOATIME` if access timestamps aren't set (all UNIX support)
 *   - `ST_RELATIME` if relative acces time is used (all UNIX support)
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @raise ENOTSUP if /zip path
 * @cancelationpoint
 */
int statfs(const char *path, struct statfs *sf) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  union statfs_meta m;
  CheckLargeStackAllocation(&m, sizeof(m));
#pragma GCC pop_options
  int rc;
  struct ZiposUri zipname;
  BEGIN_CANCELATION_POINT;

  CheckLargeStackAllocation(&m, sizeof(m));
  if (_weaken(__zipos_parseuri) &&
      _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = enotsup();
  } else if (!IsWindows()) {
    if ((rc = sys_statfs(path, &m)) != -1) {
      statfs2cosmo(sf, &m);
    }
  } else {
    rc = sys_statfs_nt(path, sf);
  }

  END_CANCELATION_POINT;
  STRACE("statfs(%#s, [%s]) → %d% m", path, DescribeStatfs(rc, sf));
  return rc;
}

__weak_reference(statfs, statfs64);
