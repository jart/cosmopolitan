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
#include "ape/sections.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/errors.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/proc/ntspawn.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/itimer.internal.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

void WipeKeystrokes(void);

static textwindows wontreturn void AbortFork(const char *func) {
#if SYSDEBUG
  kprintf("fork() %s() failed with win32 error %d\n", func, GetLastError());
#endif
  TerminateThisProcess(SIGSTKFLT);
}

static textwindows char16_t *ParseInt(char16_t *p, int64_t *x) {
  *x = 0;
  while (*p == ' ') p++;
  while ('0' <= *p && *p <= '9') {
    *x *= 10;
    *x += *p++ - '0';
  }
  return p;
}

static inline textwindows ssize_t ForkIo(int64_t h, char *p, size_t n,
                                         bool32 (*f)(int64_t, void *, uint32_t,
                                                     uint32_t *,
                                                     struct NtOverlapped *)) {
  size_t i;
  uint32_t x;
  for (i = 0; i < n; i += x) {
    if (!f(h, p + i, n - i, &x, NULL)) {
      return __winerr();
    }
  }
  return i;
}

static dontinline textwindows bool ForkIo2(int64_t h, void *buf, size_t n,
                                           bool32 (*fn)(int64_t, void *,
                                                        uint32_t, uint32_t *,
                                                        struct NtOverlapped *),
                                           const char *sf, bool ischild) {
  ssize_t rc = ForkIo(h, buf, n, fn);
  if (ischild) __tls_enabled_set(false);  // prevent tls crash in kprintf
  NTTRACE("%s(%ld, %p, %'zu) → %'zd% m", sf, h, buf, n, rc);
  return rc != -1;
}

static dontinline textwindows bool WriteAll(int64_t h, void *buf, size_t n) {
  bool ok;
  ok = ForkIo2(h, buf, n, (void *)WriteFile, "WriteFile", false);
#ifndef NDEBUG
  if (ok) ok = ForkIo2(h, &n, sizeof(n), (void *)WriteFile, "WriteFile", false);
#endif
#if SYSDEBUG
  if (!ok) {
    kprintf("failed to write %zu bytes to forked child: %d\n", n,
            GetLastError());
  }
#endif
  // Sleep(10);
  return ok;
}

static textwindows dontinline void ReadOrDie(int64_t h, void *buf, size_t n) {
  if (!ForkIo2(h, buf, n, ReadFile, "ReadFile", true)) {
    AbortFork("ReadFile1");
  }
#ifndef NDEBUG
  size_t got;
  if (!ForkIo2(h, &got, sizeof(got), ReadFile, "ReadFile", true)) {
    AbortFork("ReadFile2");
  }
  if (got != n) {
    AbortFork("ReadFile_SIZE_CHECK");
  }
#endif
}

static textwindows int64_t MapOrDie(uint32_t prot, uint64_t size) {
  int64_t h;
  for (;;) {
    if ((h = CreateFileMapping(-1, 0, prot, size >> 32, size, 0))) {
      return h;
    }
    if (GetLastError() == kNtErrorAccessDenied) {
      switch (prot) {
        case kNtPageExecuteWritecopy:
          prot = kNtPageWritecopy;
          continue;
        case kNtPageExecuteReadwrite:
          prot = kNtPageReadwrite;
          continue;
        case kNtPageExecuteRead:
          prot = kNtPageReadonly;
          continue;
        default:
          break;
      }
    }
    AbortFork("MapOrDie");
  }
}

static textwindows void ViewOrDie(int64_t h, uint32_t access, size_t pos,
                                  size_t size, void *base) {
TryAgain:
  if (!MapViewOfFileEx(h, access, pos >> 32, pos, size, base)) {
    if ((access & kNtFileMapExecute) &&
        GetLastError() == kNtErrorAccessDenied) {
      access &= ~kNtFileMapExecute;
      goto TryAgain;
    }
    AbortFork("ViewOrDie");
  }
}

static __msabi textwindows int OnForkCrash(struct NtExceptionPointers *ep) {
  kprintf("error: fork() child crashed!%n"
          "\tExceptionCode = %#x%n"
          "\tRip = %x%n",
          ep->ExceptionRecord->ExceptionCode,
          ep->ContextRecord ? ep->ContextRecord->Rip : -1);
  TerminateThisProcess(SIGSTKFLT);
}

textwindows void WinMainForked(void) {
  jmp_buf jb;
  int64_t reader;
  int64_t savetsc;
  char *addr, *shad;
  uint64_t size, upsize;
  struct MemoryInterval *maps;
  char16_t fvar[21 + 1 + 21 + 1];
  uint32_t i, varlen, oldprot, savepid;
  long mapcount, mapcapacity, specialz;
  struct Fds *fds = __veil("r", &g_fds);

  // check to see if the process was actually forked
  // this variable should have the pipe handle numba
  varlen = GetEnvironmentVariable(u"_FORK", fvar, ARRAYLEN(fvar));
  if (!varlen || varlen >= ARRAYLEN(fvar)) return;
  NTTRACE("WinMainForked()");
  SetEnvironmentVariable(u"_FORK", NULL);
#if SYSDEBUG
  int64_t oncrash = AddVectoredExceptionHandler(1, (void *)OnForkCrash);
#endif
  ParseInt(fvar, &reader);

  // read the cpu state from the parent process & plus
  // read the list of mappings from the parent process
  // this is stored in a special secretive memory map!
  // read ExtendMemoryIntervals for further details :|
  maps = (void *)kMemtrackStart;
  ReadOrDie(reader, jb, sizeof(jb));
  ReadOrDie(reader, &mapcount, sizeof(_mmi.i));
  ReadOrDie(reader, &mapcapacity, sizeof(_mmi.n));
  specialz = ROUNDUP(mapcapacity * sizeof(_mmi.p[0]), kMemtrackGran);
  ViewOrDie(MapOrDie(kNtPageReadwrite, specialz), kNtFileMapWrite, 0, specialz,
            maps);
  ReadOrDie(reader, maps, mapcount * sizeof(_mmi.p[0]));
  if (IsAsan()) {
    shad = (char *)(((intptr_t)maps >> 3) + 0x7fff8000);
    size = ROUNDUP(specialz >> 3, FRAMESIZE);
    ViewOrDie(MapOrDie(kNtPageReadwrite, size), kNtFileMapWrite, 0, size, shad);
    ReadOrDie(reader, shad, (mapcount * sizeof(_mmi.p[0])) >> 3);
  }

  // read the heap mappings from the parent process
  for (i = 0; i < mapcount; ++i) {
    addr = (char *)((uint64_t)maps[i].x << 16);
    size = maps[i].size;
    if ((maps[i].flags & MAP_TYPE) != MAP_SHARED) {
      upsize = ROUNDUP(size, FRAMESIZE);
      // we don't need to close the map handle because sys_mmap_nt
      // doesn't mark it inheritable across fork() for MAP_PRIVATE
      ViewOrDie((maps[i].h = MapOrDie(kNtPageExecuteReadwrite, upsize)),
                kNtFileMapWrite | kNtFileMapExecute, 0, upsize, addr);
      ReadOrDie(reader, addr, size);
    } else {
      // we can however safely inherit MAP_SHARED with zero copy
      ViewOrDie(maps[i].h,
                maps[i].readonlyfile ? kNtFileMapRead | kNtFileMapExecute
                                     : kNtFileMapWrite | kNtFileMapExecute,
                maps[i].offset, size, addr);
    }
  }

  // read the .data and .bss program image sections
  savepid = __pid;
  savetsc = kStartTsc;
  ReadOrDie(reader, __data_start, __data_end - __data_start);
  ReadOrDie(reader, __bss_start, __bss_end - __bss_start);
  __pid = savepid;
  kStartTsc = savetsc;
  __threaded = false;
  __tls_index = 0;
  __tls_enabled_set(false);

  // apply fixups and reapply memory protections
  _mmi.p = maps;
  _mmi.n = specialz / sizeof(_mmi.p[0]);
  for (i = 0; i < mapcount; ++i) {
    if (!VirtualProtect((void *)((uint64_t)maps[i].x << 16), maps[i].size,
                        __prot2nt(maps[i].prot, maps[i].iscow), &oldprot)) {
      AbortFork("VirtualProtect");
    }
  }

  // mitosis complete
  if (!CloseHandle(reader)) {
    AbortFork("CloseHandle");
  }

  // rewrap the stdin named pipe hack
  // since the handles closed on fork
  fds->p[0].handle = GetStdHandle(kNtStdInputHandle);
  fds->p[1].handle = GetStdHandle(kNtStdOutputHandle);
  fds->p[2].handle = GetStdHandle(kNtStdErrorHandle);

  // restore the crash reporting stuff
#if SYSDEBUG
  RemoveVectoredExceptionHandler(oncrash);
#endif
  if (_weaken(__sig_init)) {
    _weaken(__sig_init)();
  }

  // jump back into function below
  longjmp(jb, 1);
}

textwindows int sys_fork_nt(uint32_t dwCreationFlags) {
  char ok;
  jmp_buf jb;
  uint32_t op;
  char **args;
  int i, rc = -1;
  struct Proc *proc;
  struct CosmoTib *tib;
  char16_t pipename[64];
  int64_t reader, writer;
  struct NtStartupInfo startinfo;
  struct NtProcessInformation procinfo;
  char *p, forkvar[6 + 21 + 1 + 21 + 1];
  tib = __get_tls();
  ftrace_enabled(-1);
  strace_enabled(-1);
  if (!(proc = __proc_new())) return -1;
  if (!setjmp(jb)) {
    reader = CreateNamedPipe(__create_pipe_name(pipename), kNtPipeAccessInbound,
                             kNtPipeTypeByte | kNtPipeReadmodeByte, 1, PIPE_BUF,
                             PIPE_BUF, 0, &kNtIsInheritable);
    writer = CreateFile(pipename, kNtGenericWrite, 0, 0, kNtOpenExisting, 0, 0);
    if (reader != -1 && writer != -1) {
      p = stpcpy(forkvar, "_FORK=");
      p = FormatUint64(p, reader);
      bzero(&startinfo, sizeof(startinfo));
      startinfo.cb = sizeof(struct NtStartupInfo);
      startinfo.dwFlags = kNtStartfUsestdhandles;
      startinfo.hStdInput = g_fds.p[0].handle;
      startinfo.hStdOutput = g_fds.p[1].handle;
      startinfo.hStdError = g_fds.p[2].handle;
      args = __argv;
#if SYSDEBUG
      // If --strace was passed to this program, then propagate it the
      // forked process since the flag was removed by __intercept_flag
      if (strace_enabled(0) > 0) {
        int n;
        for (n = 0; args[n];) ++n;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
        int nbytes = (n + 2) * sizeof(char *);
        char **args2 = alloca(nbytes);
        CheckLargeStackAllocation(args2, nbytes);
#pragma GCC pop_options
        for (i = 0; i < n; ++i) args2[i] = args[i];
        args2[i++] = "--strace";
        args2[i] = 0;
        args = args2;
      }
#endif
      NTTRACE("STARTING SPAWN");
      int spawnrc = ntspawn(AT_FDCWD, GetProgramExecutableName(), args, environ,
                            (char *[]){forkvar, 0}, dwCreationFlags, 0, 0, 0, 0,
                            &startinfo, &procinfo);
      if (spawnrc != -1) {
        CloseHandle(procinfo.hThread);
        ok = WriteAll(writer, jb, sizeof(jb)) &&
             WriteAll(writer, &_mmi.i, sizeof(_mmi.i)) &&
             WriteAll(writer, &_mmi.n, sizeof(_mmi.n)) &&
             WriteAll(writer, _mmi.p, _mmi.i * sizeof(_mmi.p[0]));
        if (IsAsan() && ok) {
          ok = WriteAll(writer, (char *)(((intptr_t)_mmi.p >> 3) + 0x7fff8000),
                        (_mmi.i * sizeof(_mmi.p[0])) >> 3);
        }
        for (i = 0; i < _mmi.i && ok; ++i) {
          if ((_mmi.p[i].flags & MAP_TYPE) != MAP_SHARED) {
            char *p = (char *)((uint64_t)_mmi.p[i].x << 16);
            // XXX: forking destroys thread guard pages currently
            VirtualProtect(
                p, _mmi.p[i].size,
                __prot2nt(_mmi.p[i].prot | PROT_READ, _mmi.p[i].iscow), &op);
            ok = WriteAll(writer, p, _mmi.p[i].size);
          }
        }
        if (ok) ok = WriteAll(writer, __data_start, __data_end - __data_start);
        if (ok) ok = WriteAll(writer, __bss_start, __bss_end - __bss_start);
        if (ok) {
          if (!CloseHandle(writer)) ok = false;
          writer = -1;
        }
        if (ok) {
          proc->wasforked = true;
          proc->handle = procinfo.hProcess;
          rc = proc->pid = procinfo.dwProcessId;
          __proc_add(proc);
        } else {
          TerminateProcess(procinfo.hProcess, SIGKILL);
          CloseHandle(procinfo.hProcess);
        }
      }
    }
    if (reader != -1) CloseHandle(reader);
    if (writer != -1) CloseHandle(writer);
    if (rc == -1 && errno != ENOMEM) {
      eagain();  // posix fork() only specifies two errors
    }
  } else {
    rc = 0;
    // re-apply code morphing for thread-local storage
    __set_tls(tib);
    __morph_tls();
    __tls_enabled_set(true);
    // the child's pending signals is initially empty
    atomic_store_explicit(&__sig.pending, 0, memory_order_relaxed);
    atomic_store_explicit(&tib->tib_sigpending, 0, memory_order_relaxed);
    // re-enable threads
    __enable_threads();
    // re-apply code morphing for function tracing
    if (ftrace_stackdigs) {
      _weaken(__hook)(_weaken(ftrace_hook), _weaken(GetSymbolTable)());
    }
    // reset core runtime services
    __proc_wipe();
    WipeKeystrokes();
    if (_weaken(__itimer_wipe)) {
      _weaken(__itimer_wipe)();
    }
  }
  if (rc == -1) {
    dll_make_first(&__proc.free, &proc->elem);
  }
  ftrace_enabled(+1);
  strace_enabled(+1);
  return rc;
}

#endif /* __x86_64__ */
