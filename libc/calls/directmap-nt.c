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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

textwindows struct DirectMap sys_mmap_nt(void *addr, size_t size, int prot,
                                         int flags, int fd, int64_t off) {
  size_t i;
  bool iscow;
  int64_t handle;
  uint32_t oldprot;
  struct DirectMap dm;
  struct ProtectNt fl;
  const struct NtSecurityAttributes *sec;

#ifndef NDEBUG
  struct NtProcessMemoryCountersEx memcount = {
      .cb = sizeof(struct NtProcessMemoryCountersEx),
  };
  if (GetProcessMemoryInfo(GetCurrentProcess(), &memcount, sizeof(memcount))) {
    if (memcount.PeakWorkingSetSize > 5ull * 1024 * 1024 * 1024) {
      kprintf("error: exceeded 5gb memory limit%n");
      _Exit(201);
    }
  }
#endif

  if (fd != -1) {
    handle = g_fds.p[fd].handle;
  } else {
    handle = kNtInvalidHandleValue;
  }

  if (flags & MAP_PRIVATE) {
    sec = 0;  // MAP_PRIVATE isn't inherited across fork()
  } else {
    sec = &kNtIsInheritable;  // MAP_SHARED gives us zero-copy fork()
  }

  // nt will whine under many circumstances if we change the execute bit
  // later using mprotect(). the workaround is to always request execute
  // and then virtualprotect() it away until we actually need it. please
  // note that open-nt.c always requests an kNtGenericExecute accessmask
  iscow = false;
  if (handle != -1) {
    if (flags & MAP_PRIVATE) {
      // windows has cow pages but they can't propagate across fork()
      // that means we only get copy-on-write for the root process :(
      fl = (struct ProtectNt){kNtPageExecuteWritecopy,
                              kNtFileMapCopy | kNtFileMapExecute};
      iscow = true;
    } else {
      assert(flags & MAP_SHARED);
      if ((g_fds.p[fd].flags & O_ACCMODE) == O_RDONLY) {
        fl = (struct ProtectNt){kNtPageExecuteRead,
                                kNtFileMapRead | kNtFileMapExecute};
      } else {
        fl = (struct ProtectNt){kNtPageExecuteReadwrite,
                                kNtFileMapWrite | kNtFileMapExecute};
      }
    }
  } else {
    assert(flags & MAP_ANONYMOUS);
    fl = (struct ProtectNt){kNtPageExecuteReadwrite,
                            kNtFileMapWrite | kNtFileMapExecute};
  }

  if ((dm.maphandle = CreateFileMapping(handle, sec, fl.flags1,
                                        (size + off) >> 32, (size + off), 0))) {
    if ((dm.addr = MapViewOfFileEx(dm.maphandle, fl.flags2, off >> 32, off,
                                   size, addr))) {
      if (VirtualProtect(addr, size, __prot2nt(prot, iscow), &oldprot)) {
        return dm;
      } else {
        return dm;
        UnmapViewOfFile(dm.addr);
      }
    }
    CloseHandle(dm.maphandle);
  }

  dm.maphandle = kNtInvalidHandleValue;
  dm.addr = (void *)(intptr_t)-1;
  return dm;
}
