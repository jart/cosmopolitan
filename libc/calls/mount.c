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
#include "libc/calls/mount.h"
#include "libc/dce.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

int32_t sys_mount_linux(const char *source, const char *target,
                        const char *filesystemtype, uint64_t mountflags,
                        const void *data) asm("sys_mount");
int32_t sys_mount_bsd(const char *type, const char *dir, int32_t flags,
                      const void *data) asm("sys_mount");

/**
 * Mounts file system.
 *
 * The following flags may be specified:
 *
 * - `MS_RDONLY` (mount read-only)
 * - `MS_NOSUID` (don't honor S_ISUID bit)
 * - `MS_NODEV` (disallow special files)
 * - `MS_NOEXEC` (disallow program execution)
 * - `MS_SYNCHRONOUS` (writes are synced at once)
 * - `MS_NOATIME` (do not update access times)
 * - `MS_REMOUNT` (tune existing mounting)
 *
 * The following flags may also be used, but could be set to zero at
 * runtime if the underlying kernel doesn't support them.
 *
 * - `MNT_ASYNC` (xnu, freebsd, openbsd, netbsd)
 * - `MNT_RELOAD` (xnu, freebsd, openbsd, netbsd)
 * - `MS_STRICTATIME` (linux, xnu)
 * - `MS_RELATIME` (linux, netbsd)
 * - `MNT_SNAPSHOT` (xnu, freebsd)
 * - `MS_MANDLOCK` (linux)
 * - `MS_DIRSYNC` (linux)
 * - `MS_NODIRATIME` (linux)
 * - `MS_BIND` (linux)
 * - `MS_MOVE` (linux)
 * - `MS_REC` (linux)
 * - `MS_SILENT` (linux)
 * - `MS_POSIXACL` (linux)
 * - `MS_UNBINDABLE` (linux)
 * - `MS_PRIVATE` (linux)
 * - `MS_SLAVE` (linux)
 * - `MS_SHARED` (linux)
 * - `MS_KERNMOUNT` (linux)
 * - `MS_I_VERSION` (linux)
 * - `MS_LAZYTIME` (linux)
 * - `MS_ACTIVE` (linux)
 * - `MS_NOUSER` (linux)
 * - `MS_RMT`_MASK (linux)
 * - `MNT_SUIDDIR` (freebsd)
 * - `MNT_NOCLUSTERR` (freebsd)
 * - `MNT_NOCLUSTERW` (freebsd)
 *
 * Some example values for the `type` parameter:
 *
 * - `"nfs"`
 * - `"vfat"`
 * - `"tmpfs"`
 * - `"iso8601"`
 *
 */
int mount(const char *source, const char *target, const char *type,
          unsigned long flags, const void *data) {
  if (!IsWindows()) {
    if (!IsBsd()) {
      return sys_mount_linux(source, target, type, flags, data);
    } else {
      if (!strcmp(type, "iso9660")) type = "cd9660";
      if (!strcmp(type, "vfat")) {
        if (IsOpenbsd() || IsNetbsd()) {
          type = "msdos";
        } else {
          type = "msdosfs";
        }
      }
      return sys_mount_bsd(type, target, flags, data);
    }
  } else {
    return enosys();
  }
}
