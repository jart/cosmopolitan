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
#include "libc/calls/internal.h"
#include "libc/calls/ntspawn.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

STATIC_YOINK("_check_sigchld");

extern int64_t __wincrashearly;
bool32 __onntconsoleevent_nt(uint32_t);
void kmalloc_unlock(void);

static textwindows wontreturn void AbortFork(const char *func) {
  STRACE("fork() %s() failed %d", func, GetLastError());
  ExitProcess(177);
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
                                         bool32 (*f)()) {
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
                                           bool32 (*fn)(), const char *sf,
                                           bool ischild) {
  ssize_t rc = ForkIo(h, buf, n, fn);
  if (ischild) __tls_enabled = false;  // prevent tls crash in kprintf
  NTTRACE("%s(%ld, %p, %'zu) → %'zd% m", sf, h, buf, n, rc);
  return rc != -1;
}

static dontinline textwindows bool WriteAll(int64_t h, void *buf, size_t n) {
  return ForkIo2(h, buf, n, WriteFile, "WriteFile", false);
}

static textwindows dontinline void ReadOrDie(int64_t h, void *buf, size_t n) {
  if (!ForkIo2(h, buf, n, ReadFile, "ReadFile", true)) {
    AbortFork("ReadFile");
  }
}

static textwindows int64_t MapOrDie(uint32_t prot, uint64_t size) {
  int64_t h;
  if ((h = CreateFileMapping(-1, 0, prot, size >> 32, size, 0))) {
    return h;
  } else {
    AbortFork("MapOrDie");
  }
}

static textwindows void ViewOrDie(int64_t h, uint32_t access, size_t pos,
                                  size_t size, void *base) {
  void *got;
  got = MapViewOfFileEx(h, access, pos >> 32, pos, size, base);
  if (!got || (base && got != base)) {
    AbortFork("ViewOrDie");
  }
}

static textwindows int OnForkCrash(struct NtExceptionPointers *ep) {
  kprintf("error: fork() child crashed!%n"
          "\tExceptionCode = %#x%n"
          "\tRip = %x%n",
          ep->ExceptionRecord->ExceptionCode,
          ep->ContextRecord ? ep->ContextRecord->Rip : -1);
  ExitProcess(73);
}

textwindows void WinMainForked(void) {
  bool ok;
  jmp_buf jb;
  int64_t reader;
  char *addr, *shad;
  struct DirectMap dm;
  uint64_t size, upsize;
  int64_t oncrash, savetsc;
  struct MemoryInterval *maps;
  char16_t fvar[21 + 1 + 21 + 1];
  uint32_t i, varlen, oldprot, savepid;
  long mapcount, mapcapacity, specialz;

  // check to see if the process was actually forked
  // this variable should have the pipe handle numba
  varlen = GetEnvironmentVariable(u"_FORK", fvar, ARRAYLEN(fvar));
  if (!varlen || varlen >= ARRAYLEN(fvar)) return;
  NTTRACE("WinMainForked()");
  SetEnvironmentVariable(u"_FORK", NULL);
#ifdef SYSDEBUG
  oncrash = AddVectoredExceptionHandler(1, NT2SYSV(OnForkCrash));
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
    ViewOrDie(MapOrDie(kNtPageReadwrite, size), kNtFileMapWrite, 0, size, maps);
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
  __tls_enabled = false;

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
  struct Fds *fds = VEIL("r", &g_fds);
  fds->p[0].handle = GetStdHandle(kNtStdInputHandle);
  fds->p[1].handle = GetStdHandle(kNtStdOutputHandle);
  fds->p[2].handle = GetStdHandle(kNtStdErrorHandle);

  // untrack children of parent since we specify with both
  // CreateProcess() and CreateThread() as non-inheritable
  for (i = 0; i < fds->n; ++i) {
    if (fds->p[i].kind == kFdProcess) {
      fds->p[i].kind = 0;
      atomic_store_explicit(&fds->f, MIN(i, fds->f), memory_order_relaxed);
    }
  }

  // restore the crash reporting stuff
#ifdef SYSDEBUG
  RemoveVectoredExceptionHandler(oncrash);
#endif
  if (_weaken(__wincrash_nt)) {
    if (!IsTiny()) {
      RemoveVectoredExceptionHandler(__wincrashearly);
    }
    AddVectoredExceptionHandler(1, (void *)_weaken(__wincrash_nt));
  }
  if (_weaken(__onntconsoleevent_nt)) {
    SetConsoleCtrlHandler(_weaken(__onntconsoleevent_nt), 1);
  }

  // jump back into function below
  longjmp(jb, 1);
}

textwindows int sys_fork_nt(uint32_t dwCreationFlags) {
  jmp_buf jb;
  uint32_t oldprot;
  char ok, threaded;
  char **args, **args2;
  struct CosmoTib *tib;
  char16_t pipename[64];
  int64_t reader, writer;
  struct NtStartupInfo startinfo;
  int i, n, pid, untrackpid, rc = -1;
  char *p, forkvar[6 + 21 + 1 + 21 + 1];
  struct NtProcessInformation procinfo;
  threaded = __threaded;
  tib = __tls_enabled ? __get_tls() : 0;
  if (!setjmp(jb)) {
    pid = untrackpid = __reservefd_unlocked(-1);
    reader = CreateNamedPipe(CreatePipeName(pipename),
                             kNtPipeAccessInbound | kNtFileFlagOverlapped,
                             kNtPipeTypeMessage | kNtPipeReadmodeMessage, 1,
                             65536, 65536, 0, &kNtIsInheritable);
    writer = CreateFile(pipename, kNtGenericWrite, 0, 0, kNtOpenExisting,
                        kNtFileFlagOverlapped, 0);
    if (pid != -1 && reader != -1 && writer != -1) {
      p = stpcpy(forkvar, "_FORK=");
      p = FormatUint64(p, reader);
      bzero(&startinfo, sizeof(startinfo));
      startinfo.cb = sizeof(struct NtStartupInfo);
      startinfo.dwFlags = kNtStartfUsestdhandles;
      startinfo.hStdInput = __getfdhandleactual(0);
      startinfo.hStdOutput = __getfdhandleactual(1);
      startinfo.hStdError = __getfdhandleactual(2);
      args = __argv;
#ifdef SYSDEBUG
      // If --strace was passed to this program, then propagate it the
      // forked process since the flag was removed by __intercept_flag
      if (strace_enabled(0) > 0) {
        for (n = 0; args[n];) ++n;
        args2 = alloca((n + 2) * sizeof(char *));
        for (i = 0; i < n; ++i) args2[i] = args[i];
        args2[i++] = "--strace";
        args2[i] = 0;
        args = args2;
      }
#endif
      if (ntspawn(GetProgramExecutableName(), args, environ, forkvar, 0, 0,
                  true, dwCreationFlags, 0, &startinfo, &procinfo) != -1) {
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
            ok = WriteAll(writer, (void *)((uint64_t)_mmi.p[i].x << 16),
                          _mmi.p[i].size);
          }
        }
        if (ok) ok = WriteAll(writer, __data_start, __data_end - __data_start);
        if (ok) ok = WriteAll(writer, __bss_start, __bss_end - __bss_start);
        if (ok) {
          if (!CloseHandle(writer)) ok = false;
          writer = -1;
        }
        if (ok) {
          g_fds.p[pid].kind = kFdProcess;
          g_fds.p[pid].handle = procinfo.hProcess;
          g_fds.p[pid].flags = O_CLOEXEC;
          g_fds.p[pid].zombie = false;
          untrackpid = -1;
          rc = pid;
        } else {
          TerminateProcess(procinfo.hProcess, 127);
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
    if (tib && _weaken(__set_tls) && _weaken(__morph_tls)) {
      _weaken(__set_tls)(tib);
      _weaken(__morph_tls)();
      __tls_enabled = true;
    }
    if (threaded && !__threaded && _weaken(__enable_threads)) {
      _weaken(__enable_threads)();
    }
  }
  if (untrackpid != -1) {
    __releasefd(untrackpid);
  }
  return rc;
}
