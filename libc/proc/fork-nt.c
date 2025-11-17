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
#include "libc/calls/flocks.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/directmap.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/mem/alloca.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
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
#include "libc/sysv/pib.h"
#include "libc/thread/tls.h"
#if SupportsWindows()

extern long __klog_handle;
extern bool __winmain_isfork;
extern intptr_t __winmain_jmpbuf[5];
extern struct CosmoTib *__winmain_tib;

__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;
__msabi extern typeof(TerminateProcess) *const __imp_TerminateProcess;
__msabi extern typeof(TlsAlloc) *const __imp_TlsAlloc;
__msabi extern typeof(VirtualProtectEx) *const __imp_VirtualProtectEx;

static textwindows void *sys_fork_nt_malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

static textwindows void sys_fork_nt_free(void *ptr) {
  HeapFree(GetProcessHeap(), 0, ptr);
}

textwindows wontreturn static void AbortFork(const char *func, void *addr) {
#if SYSDEBUG
  kprintf("fork() %!s(%lx) failed with win32 error %u\n", func, addr,
          GetLastError());
#endif
  __imp_TerminateProcess(-1, SIGURG);
  __builtin_unreachable();
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
  __set_tls_win32(__winmain_tib);
  __tls_enabled = true;

  // forget posix advisory locks
  if (_weaken(__flocks_wipe))
    _weaken(__flocks_wipe)();

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
  __get_pib()->fds.p[0].handle = GetStdHandle(kNtStdInputHandle);
  __get_pib()->fds.p[1].handle = GetStdHandle(kNtStdOutputHandle);
  __get_pib()->fds.p[2].handle = GetStdHandle(kNtStdErrorHandle);
}

textwindows static int sys_fork_nt_parent(int child_pid, intptr_t hStopEvent) {

  // allocate process object
  struct Proc *proc;
  if (!(proc = __proc_new()))
    return -1;
  proc->hStopEvent = hStopEvent;

  // get path of this executable
  char16_t prog[PATH_MAX];
  unsigned got = GetModuleFileName(0, prog, ARRAYLEN(prog));
  if (!got || got >= ARRAYLEN(prog)) {
    dll_make_first(&__proc.free, &proc->elem);
    enomem();
    return -1;
  }

  // taint all cursors so they can't be cached
  for (int fd = 0; fd < __get_pib()->fds.n; ++fd)
    if (__get_pib()->fds.p[fd].kind == kFdFile)
      if (__get_pib()->fds.p[fd].cursor)
        __get_pib()->fds.p[fd].cursor->is_multiprocess = true;

  // spawn new process in suspended state
  struct NtProcessInformation procinfo;
  struct NtStartupInfo startinfo = {
      .cb = sizeof(struct NtStartupInfo),
      .dwFlags = kNtStartfUsestdhandles,
      .hStdInput = __get_pib()->fds.p[0].handle,
      .hStdOutput = __get_pib()->fds.p[1].handle,
      .hStdError = __get_pib()->fds.p[2].handle,
  };
  char16_t *cwd = sys_fork_nt_malloc(PATH_MAX * sizeof(char16_t));
  GetCurrentDirectory(PATH_MAX, cwd);
  if (!CreateProcess(prog, 0, 0, 0, true,
                     __get_pib()->dwCreationFlags | kNtCreateSuspended |
                         kNtInheritParentAffinity |
                         kNtCreateUnicodeEnvironment |
                         GetPriorityClass(GetCurrentProcess()),
                     0, cwd, &startinfo, &procinfo)) {
    sys_fork_nt_free(cwd);
    STRACE("fork() %s() failed w/ %m %d", "CreateProcess", GetLastError());
    dll_make_first(&__proc.free, &proc->elem);
    if (errno != ENOMEM)
      eagain();
    return -1;
  }
  sys_fork_nt_free(cwd);

  // let's go
  bool ok = true;

  // copy memory manager maps
  for (struct MapSlab *slab =
           atomic_load_explicit(&__maps.slabs, memory_order_acquire);
       slab->next;  // exclude __maps.slab1 which is static memory
       slab = slab->next) {
    ok = ok && !!VirtualAllocEx(procinfo.hProcess, slab, MAPS_SLAB_SIZE,
                                kNtMemReserve | kNtMemCommit, kNtPageReadwrite);
    ok = ok &&
         !!WriteProcessMemory(procinfo.hProcess, slab, slab, MAPS_SLAB_SIZE, 0);
  }

  // copy private memory maps
  int alloc_prot = -1;
  for (struct Map *map = __maps_first(); map; map = __maps_next(map)) {
    if ((map->flags & MAP_TYPE) == MAP_SHARED)
      continue;  // shared memory doesn't need to be copied to subprocess
    if ((map->flags & MAP_NOFORK) && (map->flags & MAP_TYPE) != MAP_FILE)
      continue;  // ignore things like signal worker stack memory
    if ((map->flags & MAP_NOFORK) && map->prot == PROT_READ)
      continue;  // we don't need to copy executable .rodata
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
    proc->hProcess = procinfo.hProcess;
    proc->pid = child_pid;
    __proc_add(proc);
    return child_pid;
  } else {
    if (errno != ENOMEM)
      eagain();  // posix fork() only specifies two errors
    TerminateProcess(procinfo.hProcess, SIGKILL);
    CloseHandle(procinfo.hProcess);
    dll_make_first(&__proc.free, &proc->elem);
    return -1;
  }
}

textwindows int sys_fork_nt(void) {
  int rc;
  static int child_pid;
  static intptr_t hStopEvent;
  atomic_ulong *child_sigpending;
  if (!(hStopEvent = CreateEvent(&kNtIsInheritable, 1, 0, 0)))  // manual reset
    return enomem();
  child_pid = __generate_pid(&child_sigpending);
  UnmapViewOfFile(child_sigpending);
  __winmain_isfork = true;
  __winmain_tib = __get_tls_win32();
  if (!__builtin_setjmp(__winmain_jmpbuf)) {
    rc = sys_fork_nt_parent(child_pid, hStopEvent);
    if (rc == -1)
      DeleteFile(__sig_process_path(alloca(256), child_pid));
  } else {
    sys_fork_nt_child();
    __get_pib()->pid = child_pid;
    __get_pib()->sigpending = __sig_map_process(child_pid, kNtOpenAlways);
    __get_pib()->hStopEvent = hStopEvent;
    __get_pib()->hStopChurn = __proc.hStopChurn;
    rc = 0;
  }
  __winmain_isfork = false;
  return rc;
}

#endif /* __x86_64__ */
