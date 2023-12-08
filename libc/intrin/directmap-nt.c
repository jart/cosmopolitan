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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/errno.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

textwindows struct DirectMap sys_mmap_nt(void *addr, size_t size, int prot,
                                         int flags, int fd, int64_t off) {

  int64_t handle;
  if (flags & MAP_ANONYMOUS) {
    handle = kNtInvalidHandleValue;
  } else {
    handle = g_fds.p[fd].handle;
  }

  // mark map handle as inheritable if fork might need it
  const struct NtSecurityAttributes *mapsec;
  if ((flags & MAP_TYPE) == MAP_SHARED) {
    mapsec = &kNtIsInheritable;
  } else {
    mapsec = 0;
  }

  // nt will whine under many circumstances if we change the execute bit
  // later using mprotect(). the workaround is to always request execute
  // and then virtualprotect() it away until we actually need it. please
  // note that open-nt.c always requests an kNtGenericExecute accessmask
  int iscow = false;
  struct ProtectNt fl;
  if (handle != -1) {
    if ((flags & MAP_TYPE) != MAP_SHARED) {
      // windows has cow pages but they can't propagate across fork()
      // that means we only get copy-on-write for the root process :(
      fl = (struct ProtectNt){kNtPageExecuteWritecopy,
                              kNtFileMapCopy | kNtFileMapExecute};
      iscow = true;
    } else {
      if ((g_fds.p[fd].flags & O_ACCMODE) == O_RDONLY) {
        fl = (struct ProtectNt){kNtPageExecuteRead,
                                kNtFileMapRead | kNtFileMapExecute};
      } else {
        fl = (struct ProtectNt){kNtPageExecuteReadwrite,
                                kNtFileMapWrite | kNtFileMapExecute};
      }
    }
  } else {
    unassert(flags & MAP_ANONYMOUS);
    fl = (struct ProtectNt){kNtPageExecuteReadwrite,
                            kNtFileMapWrite | kNtFileMapExecute};
  }

  int e = errno;
  struct DirectMap dm;
TryAgain:
  if ((dm.maphandle = CreateFileMapping(handle, mapsec, fl.flags1,
                                        (size + off) >> 32, (size + off), 0))) {
    if ((dm.addr = MapViewOfFileEx(dm.maphandle, fl.flags2, off >> 32, off,
                                   size, addr))) {
      uint32_t oldprot;
      if (VirtualProtect(addr, size, __prot2nt(prot, iscow), &oldprot)) {
        return dm;
      }
      UnmapViewOfFile(dm.addr);
    }
    CloseHandle(dm.maphandle);
  } else if (!(prot & PROT_EXEC) &&              //
             (fl.flags2 & kNtFileMapExecute) &&  //
             GetLastError() == kNtErrorAccessDenied) {
    // your file needs to have been O_CREAT'd with exec `mode` bits in
    // order to be mapped with executable permission. we always try to
    // get execute permission if the kernel will give it to us because
    // win32 would otherwise forbid mprotect() from elevating later on
    fl.flags2 &= ~kNtFileMapExecute;
    switch (fl.flags1) {
      case kNtPageExecuteWritecopy:
        fl.flags1 = kNtPageWritecopy;
        break;
      case kNtPageExecuteReadwrite:
        fl.flags1 = kNtPageReadwrite;
        break;
      case kNtPageExecuteRead:
        fl.flags1 = kNtPageReadonly;
        break;
      default:
        __builtin_unreachable();
    }
    errno = e;
    goto TryAgain;
  }

  dm.maphandle = kNtInvalidHandleValue;
  dm.addr = (void *)(intptr_t)-1;
  return dm;
}
