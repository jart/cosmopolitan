// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/sysv/errfuns.h"

#define BIG_REPARSE_DATA_BUFFER_SIZE 16384

struct ReadSymlinkNt {
  union {
    struct NtReparseDataBuffer rdb;
    char rdb_bytes[BIG_REPARSE_DATA_BUFFER_SIZE];
  };
};

textwindows static int __readntsym_impl(intptr_t hSymlink,
                                        char path8[static PATH_MAX],
                                        uint32_t *opt_out_ReparseTag,
                                        struct ReadSymlinkNt *m) {

  // get handle for overlapped i/o
  uintptr_t hEvent;
  if (!(hEvent = CreateEventTls()))
    return enomem();
  uint32_t dwBytesReturned;
  struct NtOverlapped overlap = {.hEvent = hEvent};

  // todo: is it possible for a reparse point to hold multiple buffers
  bool32 ok =
      DeviceIoControl(hSymlink, kNtFsctlGetReparsePoint, 0, 0, &m->rdb,
                      BIG_REPARSE_DATA_BUFFER_SIZE, &dwBytesReturned, &overlap);

  // functions like fstat() aren't cancelation points (thank goodness)
  // so we can skip most of the overlapped i/o ceremony
  if (!ok && GetLastError() == kNtErrorIoPending)
    ok = GetOverlappedResult(hSymlink, &overlap, &dwBytesReturned, true);
  CloseEventTls(hEvent);
  if (!ok)
    return eacces();

  // reparse points can serve many purposes besides just symbolic links
  if (m->rdb.ReparseTag != kNtIoReparseTagSymlink) {
    // e.g. 0xA000001D means IO_REPARSE_TAG_LX_SYMLINK
    //
    //     "WSL symlinks can't be opened from Windows, only from
    //      within WSL, so if we identify them as fs.ModeSymlink,
    //      then functions like filepath.Walk would fail when trying
    //      to follow the link."
    //
    //            —Quoth Quim Muntal (dev on Go team at Microsoft)
    //
    // See also MSDN Learn § 2.1.2.1 Reparse Tags. There's also
    // 0x80000023 which means IO_REPARSE_TAG_AF_UNIX, which is what
    // happens when we create an AF_UNIX domain socket.
    if (m->rdb.ReparseTag != kNtIoReparseTagAfUnix)
      NTTRACE("unexpected reparse point tag %#x");
    if (opt_out_ReparseTag)
      *opt_out_ReparseTag = m->rdb.ReparseTag;
    return enolink();
  }

  // convert ntdll style target path into utf-8 unix style path
  int path16len =
      m->rdb.SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(char16_t);
  char16_t *path16 =
      (char16_t *)((char *)m->rdb.SymbolicLinkReparseBuffer.PathBuffer +
                   m->rdb.SymbolicLinkReparseBuffer.SubstituteNameOffset);
  if (path16len >= PATH_MAX ||
      sizeof(struct NtReparseDataBuffer) + (path16len + 1) * sizeof(char16_t) >
          BIG_REPARSE_DATA_BUFFER_SIZE)
    return enomem();
  path16[path16len] = 0;
  return __mkunixpath(path16, path8);
}

/**
 * Reads windows symlink content from handle as utf-8 unix path.
 *
 * If this function succeeds, no truncation occurred and `path8` will be
 * nul terminated. This implementation generally maps truncation errors,
 * and path too long issues into an ENOMEM error. If for some reason the
 * handle wasn't a valid reparse point that's a symbolic link, then this
 * raises an ENOLINK error and additionally stores the reparse tag which
 * is important to consider since it can differentiate unix socket files
 *
 * @return string length of `path8` output, or -1 w/ errno
 */
textwindows int __readntsym(intptr_t hSymlink, char path8[static PATH_MAX],
                            uint32_t *opt_out_ReparseTag) {
  int path8len;
  BLOCK_SIGNALS;
  struct ReadSymlinkNt *m;
  int64_t hHeap = GetProcessHeap();
  if ((m = HeapAlloc(hHeap, 0, sizeof(struct ReadSymlinkNt)))) {
    path8len = __readntsym_impl(hSymlink, path8, opt_out_ReparseTag, m);
    HeapFree(hHeap, 0, m);
  } else {
    path8len = enomem();
  }
  ALLOW_SIGNALS;
  return path8len;
}
