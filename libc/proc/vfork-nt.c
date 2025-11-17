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

#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/proc/proc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"
#include "libc/thread/tls.h"
#if SupportsWindows()

extern struct CosmoPib __pib;

void sys_vfork_nt_longjmp(jmp_buf, int) wontreturn;

textwindows static void sys_vfork_nt_unref_cursors(struct CursorRefs r) {
  for (size_t i = 0; i < r.n; ++i)
    __cursor_unref(r.p[i]);
  free(r.p);
}

textwindows dontinline dontinstrument wontreturn void sys_vfork_nt_return(
    jmp_buf jb, int rc) {
  STRACE("vfork() → %d% m", rc);
  sys_vfork_nt_longjmp(jb, rc);
}

textwindows dontinstrument wontreturn static void sys_vfork_nt_finish(
    struct CosmoPib *self, struct CosmoTib *tib, sigset_t sigmask) {
  int pid;
  jmp_buf jb;
  intptr_t handle;
  struct CursorRefs r;
  struct Cursor *c, *c2;
  pid = self->pid;
  handle = self->proc->hProcess;
  for (int fd = 0; fd < self->fds.n; ++fd)
    if (self->fds.p[fd].kind != kFdEmpty)
      if (self->fds.p[fd].kind != kFdReserved)
        if (self->fds.p[fd].was_created_during_vfork)
          close(fd);
  tib->tib_flags &= ~TIB_FLAG_VFORKED;
  tib->tib_vfork = 0;
  munmap(self->fds.p, self->fds.c);
  r = self->refedcursors;
  c = self->fds.freed_cursors;
  memcpy(jb, self->vforkret, sizeof(jmp_buf));
  free(self);
  sys_vfork_nt_unref_cursors(r);
  for (; c; c = c2) {
    c2 = c->next_free_cursor;
    __cursor_destroy(c);
  }
  SetEvent(handle);
  atomic_store(&tib->tib_sigmask, sigmask);
  sys_vfork_nt_return(jb, pid);
}

textwindows void sys_vfork_nt_exec(intptr_t hProcess) {
  struct CosmoTib *tib = __get_tls_win32();
  if (!(tib->tib_flags & TIB_FLAG_VFORKED))
    return;
  sigset_t sigmask = atomic_exchange(&tib->tib_sigmask, -1);
  struct CosmoPib *self = tib->tib_vfork;
  self->proc->hProcess2 = hProcess;
  sys_vfork_nt_finish(self, tib, sigmask);
}

textwindows dontinstrument void sys_vfork_nt_exit(uint32_t dwExitCode) {
  struct CosmoTib *tib = __get_tls_win32();
  if (!(tib->tib_flags & TIB_FLAG_VFORKED))
    return;
  sigset_t sigmask = atomic_exchange(&tib->tib_sigmask, -1);
  struct CosmoPib *self = tib->tib_vfork;
  self->proc->dwExitCode = dwExitCode;
  sys_vfork_nt_finish(self, tib, sigmask);
}

textwindows static int sys_vfork_nt_impl(jmp_buf jb, struct CosmoTib *tib) {
  struct CosmoPib *parent, *child;
  if (tib->tib_flags & TIB_FLAG_VFORKED)
    return enotsup();  // who would vfork in a vfork
  parent = &__pib;
  if (!(child = malloc(sizeof(struct CosmoPib))))
    return enomem();
  struct Proc *proc;
  __proc_lock();
  proc = __proc_new();
  __proc_unlock();
  if (!proc) {
  RaiseEnomem1:
    free(child);
    return enomem();
  }
  if (!(proc->hProcess = CreateEvent(0, 1, 0, 0))) {  // manual reset
  RaiseEnomem2:
    __proc_lock();
    dll_make_first(&__proc.free, &proc->elem);
    __proc_unlock();
    goto RaiseEnomem1;
  }
  if (!(proc->hStopEvent = CreateEvent(0, 1, 0, 0))) {  // manual reset
  RaiseEnomem3:
    CloseHandle(proc->hProcess);
    goto RaiseEnomem2;
  }
AllocateFdsMap:
  size_t c = parent->fds.c;
  struct Fd *fdp = mmap(__maps_randaddr(), c, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (fdp == MAP_FAILED) {
  RaiseEnomem4:
    CloseHandle(proc->hStopEvent);
    goto RaiseEnomem3;
  }
  __fds_lock();
  memcpy(child, parent, sizeof(struct CosmoPib));
  memcpy(child->vforkret, jb, sizeof(jmp_buf));
  if (c != child->fds.c) {
    __fds_unlock();
    munmap(fdp, c);
    goto AllocateFdsMap;
  }
  child->ppid_cosmo = parent->pid;
  child->ppid_win32 = GetCurrentProcessId();
  child->hStopEvent = proc->hStopEvent;
  child->hStopChurn = __proc.hStopChurn;
  memcpy(fdp, child->fds.p, child->fds.n * sizeof(*child->fds.p));
  proc->isvfork = true;
  child->proc = proc;
  child->fds.p = fdp;
  child->fds.freed_cursors = 0;
  for (int fd = 0; fd < child->fds.n; ++fd) {
    if (child->fds.p[fd].kind == kFdEmpty)
      continue;
    if (child->fds.p[fd].kind == kFdReserved)
      continue;
    if (__cursor_ref(child->fds.p[fd].cursor)) {
      if (child->refedcursors.n == child->refedcursors.c) {
        size_t c2 = child->refedcursors.c + 4;
        c2 += c2 >> 1;
        struct Cursor **p2 =
            realloc(child->refedcursors.p, c2 * sizeof(*child->refedcursors.p));
        if (!p2) {
          __fds_unlock();
          __cursor_unref(child->fds.p[fd].cursor);
          sys_vfork_nt_unref_cursors(child->refedcursors);
          munmap(child->fds.p, child->fds.c);
          goto RaiseEnomem4;
        }
        child->refedcursors.p = p2;
        child->refedcursors.c = c2;
      }
      child->refedcursors.p[child->refedcursors.n++] = child->fds.p[fd].cursor;
    }
  }
  __fds_unlock();
  proc->pid = child->pid = __generate_pid(&child->sigpending);
  __proc_lock();
  __proc_add(proc);
  __proc_unlock();
  tib->tib_flags |= TIB_FLAG_VFORKED;
  tib->tib_vfork = child;
  return 0;
}

textwindows void sys_vfork_nt(jmp_buf jb) {
  int rc;
  struct CosmoTib *tib = __get_tls_win32();
  sigset_t sigmask = atomic_exchange(&tib->tib_sigmask, -1);
  if (_weaken(kloghandle))
    _weaken(kloghandle)();
  rc = sys_vfork_nt_impl(jb, tib);
  atomic_store(&tib->tib_sigmask, sigmask);
  sys_vfork_nt_return(jb, rc);
}

#endif /* __x86_64__ */
