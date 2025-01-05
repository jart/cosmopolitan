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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/errors.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/proc/proc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

extern long __klog_handle;
extern bool __winmain_isfork;
extern intptr_t __winmain_jmpbuf[5];
extern struct CosmoTib *__winmain_tib;

__msabi extern typeof(TlsAlloc) *const __imp_TlsAlloc;
__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;
__msabi extern typeof(VirtualProtectEx) *const __imp_VirtualProtectEx;

textwindows wontreturn static void AbortFork(const char *func, void *addr) {
#if SYSDEBUG
  kprintf("fork() %!s(%lx) failed with win32 error %u\n", func, addr,
          GetLastError());
#endif
  TerminateThisProcess(SIGSTKFLT);
}

textwindows static void ViewOrDie(int64_t h, uint32_t access, size_t pos,
                                  size_t size, void *base) {
TryAgain:
  if (!__imp_MapViewOfFileEx(h, access, pos >> 32, pos, size, base)) {
    if ((access & kNtFileMapExecute) &&
        GetLastError() == kNtErrorAccessDenied) {
      access &= ~kNtFileMapExecute;
      goto TryAgain;
    }
    AbortFork("ViewOrDie", base);
  }
}

textwindows static void sys_fork_nt_child(void) {

  // setup runtime
  __klog_handle = 0;
  __tls_index = __imp_TlsAlloc();
  __morph_tls();
  __set_tls_win32(__winmain_tib);
  __tls_enabled = true;

  // resurrect shared memory mappings
  struct Map *next;
  for (struct Map *map = __maps_first(); map; map = next) {
    next = __maps_next(map);

    // cleanup nofork mappings
    if (map->flags & MAP_NOFORK) {
      if ((map->flags & MAP_TYPE) != MAP_FILE) {
        tree_remove(&__maps.maps, &map->tree);
        __maps.pages -= (map->size + __pagesize - 1) / __pagesize;
        __maps.count -= 1;
        __maps_free(map);
      }
      continue;
    }

    // private maps already copied/protected to child by parent
    if ((map->flags & MAP_TYPE) != MAP_SHARED) {
      // it's not copy-on-write anymore
      map->iscow = false;
      // but it used VirtualAlloc() so munmap() must VirtualFree()
      if (map->hand > 0) {
        CloseHandle(map->hand);
        map->hand = MAPS_VIRTUAL;
      }
      continue;
    }

    // handle granularity aligned shared mapping
    if (__maps_isalloc(map)) {

      // get true size of win32 allocation
      size_t allocsize = map->size;
      for (struct Map *map2 = next; map2; map2 = __maps_next(map2)) {
        if (!__maps_isalloc(map2) && map->addr + allocsize == map2->addr) {
          allocsize += map2->size;
        } else {
          break;
        }
      }

      // create allocation with most permissive access possible
      // if we don't create as rwx then we can't mprotect(rwx) later
      unsigned access;
      if (map->readonlyfile) {
        access = kNtFileMapRead | kNtFileMapExecute;
      } else {
        access = kNtFileMapWrite | kNtFileMapExecute;
      }

      // resurrect copyless memory via inherited win32 handle
      ViewOrDie(map->hand, access, map->off, allocsize, map->addr);
    }

    // restore memory protection status on pages
    unsigned old_protect;
    if (!__imp_VirtualProtectEx(GetCurrentProcess(), map->addr, map->size,
                                __prot2nt(map->prot, false), &old_protect))
      AbortFork("VirtualProtectEx", map->addr);
  }

  // function tracing is now safe
  ftrace_enabled(+1);

  // initialize winsock
  void WinSockFork(void);
  if (_weaken(WinSockFork))
    _weaken(WinSockFork)();

  // rewrap the stdin named pipe hack
  // since the handles closed on fork
  g_fds.p[0].handle = GetStdHandle(kNtStdInputHandle);
  g_fds.p[1].handle = GetStdHandle(kNtStdOutputHandle);
  g_fds.p[2].handle = GetStdHandle(kNtStdErrorHandle);
}

textwindows static int sys_fork_nt_parent(uint32_t dwCreationFlags) {

  // allocate process object
  struct Proc *proc;
  if (!(proc = __proc_new()))
    return -1;

  // get path of this executable
  char16_t prog[PATH_MAX];
  unsigned got = GetModuleFileName(0, prog, ARRAYLEN(prog));
  if (!got || got >= ARRAYLEN(prog)) {
    dll_make_first(&__proc.free, &proc->elem);
    enomem();
    return -1;
  }

  // spawn new process in suspended state
  struct NtProcessInformation procinfo;
  struct NtStartupInfo startinfo = {
      .cb = sizeof(struct NtStartupInfo),
      .dwFlags = kNtStartfUsestdhandles,
      .hStdInput = g_fds.p[0].handle,
      .hStdOutput = g_fds.p[1].handle,
      .hStdError = g_fds.p[2].handle,
  };
  if (!CreateProcess(prog, 0, 0, 0, true,
                     dwCreationFlags | kNtCreateSuspended |
                         kNtInheritParentAffinity |
                         kNtCreateUnicodeEnvironment |
                         GetPriorityClass(GetCurrentProcess()),
                     0, 0, &startinfo, &procinfo)) {
    STRACE("fork() %s() failed w/ %m %d", "CreateProcess", GetLastError());
    dll_make_first(&__proc.free, &proc->elem);
    if (errno != ENOMEM)
      eagain();
    return -1;
  }

  // ensure process can be signaled before returning
  UnmapViewOfFile(__sig_map_process(procinfo.dwProcessId, kNtOpenAlways));

  // let's go
  bool ok = true;

  // copy memory manager maps
  for (struct MapSlab *slab =
           atomic_load_explicit(&__maps.slabs, memory_order_acquire);
       slab; slab = slab->next) {
    ok = ok && !!VirtualAllocEx(procinfo.hProcess, slab, MAPS_SIZE,
                                kNtMemReserve | kNtMemCommit, kNtPageReadwrite);
    ok =
        ok && !!WriteProcessMemory(procinfo.hProcess, slab, slab, MAPS_SIZE, 0);
  }

  // copy private memory maps
  int alloc_prot = -1;
  for (struct Map *map = __maps_first(); map; map = __maps_next(map)) {
    if ((map->flags & MAP_TYPE) == MAP_SHARED)
      continue;  // shared memory doesn't need to be copied to subprocess
    if ((map->flags & MAP_NOFORK) && (map->flags & MAP_TYPE) != MAP_FILE)
      continue;  // ignore things like signal worker stack memory
    if (__maps_isalloc(map)) {
      size_t allocsize = map->size;
      for (struct Map *m2 = __maps_next(map); m2; m2 = __maps_next(m2)) {
        if (!__maps_isalloc(m2) && map->addr + allocsize == m2->addr) {
          allocsize += m2->size;
        } else {
          break;
        }
      }
      if ((map->flags & MAP_NOFORK) && (map->flags & MAP_TYPE) == MAP_FILE) {
        // portable executable segment
        if (map->prot & PROT_EXEC)
          // TODO(jart): write a __remorph_tls() function
          continue;
        if (!(map->prot & PROT_WRITE)) {
          uint32_t child_old_protect;
          ok = ok && !!VirtualProtectEx(procinfo.hProcess, map->addr, allocsize,
                                        kNtPageReadwrite, &child_old_protect);
          alloc_prot = PROT_READ | PROT_WRITE;
        } else {
          alloc_prot = map->prot;
        }
      } else {
        // private mapping
        uint32_t page_flags;
        if (!(alloc_prot & PROT_WRITE)) {
          page_flags = kNtPageReadwrite;
          alloc_prot = PROT_READ | PROT_WRITE;
        } else {
          page_flags = __prot2nt(alloc_prot, false);
        }
        ok = ok && !!VirtualAllocEx(procinfo.hProcess, map->addr, allocsize,
                                    kNtMemReserve | kNtMemCommit, page_flags);
      }
    }
    uint32_t parent_old_protect;
    if (!(map->prot & PROT_READ))
      ok = ok && !!VirtualProtect(map->addr, map->size, kNtPageReadwrite,
                                  &parent_old_protect);
    ok = ok &&
         !!WriteProcessMemory(procinfo.hProcess, map->addr, map->addr,
                              (map->size + __pagesize - 1) & -__pagesize, 0);
    if (map->prot != alloc_prot) {
      uint32_t child_old_protect;
      ok = ok &&
           !!VirtualProtectEx(procinfo.hProcess, map->addr, map->size,
                              __prot2nt(map->prot, false), &child_old_protect);
    }
    if (!(map->prot & PROT_READ))
      ok = ok && !!VirtualProtect(map->addr, map->size, parent_old_protect,
                                  &parent_old_protect);
  }

  // set process loose
  ok = ok && ResumeThread(procinfo.hThread) != -1u;
  ok &= !!CloseHandle(procinfo.hThread);

  // return pid of new process
  if (ok) {
    proc->wasforked = true;
    proc->handle = procinfo.hProcess;
    proc->pid = procinfo.dwProcessId;
    __proc_add(proc);
    return procinfo.dwProcessId;
  } else {
    if (errno != ENOMEM)
      eagain();  // posix fork() only specifies two errors
    TerminateProcess(procinfo.hProcess, SIGKILL);
    CloseHandle(procinfo.hProcess);
    dll_make_first(&__proc.free, &proc->elem);
    return -1;
  }
}

textwindows int sys_fork_nt(uint32_t dwCreationFlags) {
  int rc;
  __winmain_isfork = true;
  __winmain_tib = __get_tls();
  if (!__builtin_setjmp(__winmain_jmpbuf)) {
    rc = sys_fork_nt_parent(dwCreationFlags);
  } else {
    sys_fork_nt_child();
    rc = 0;
  }
  __winmain_isfork = false;
  return rc;
}

#endif /* __x86_64__ */
