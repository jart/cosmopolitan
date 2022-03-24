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
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/calls/ntspawn.h"
#include "libc/calls/strace.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"

STATIC_YOINK("_check_sigchld");

extern int __pid;
extern unsigned long long __kbirth;
extern unsigned char __data_start[]; /* αpε */
extern unsigned char __data_end[];   /* αpε */
extern unsigned char __bss_start[];  /* αpε */
extern unsigned char __bss_end[];    /* αpε */

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
      return -1;
    }
  }
  return i;
}

static dontinline textwindows bool ForkIo2(int64_t h, void *buf, size_t n,
                                           bool32 (*fn)(), const char *sf) {
  ssize_t rc = ForkIo(h, buf, n, fn);
  STRACE("%s(%ld, %'zu) → %'zd% m", sf, h, n, rc);
  return rc != -1;
}

static dontinline textwindows bool WriteAll(int64_t h, void *buf, size_t n) {
  return ForkIo2(h, buf, n, WriteFile, "WriteFile");
}

static textwindows dontinline bool ReadAll(int64_t h, void *buf, size_t n) {
  return ForkIo2(h, buf, n, ReadFile, "ReadFile");
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
  char *addr, *shad;
  struct DirectMap dm;
  uint64_t size, upsize;
  int64_t reader, writer;
  uint32_t flags1, flags2;
  struct MemoryInterval *maps;
  char16_t fvar[21 + 1 + 21 + 1];
  int64_t oncrash, savetsc, savebir;
  uint32_t i, varlen, oldprot, savepid;
  long mapcount, mapcapacity, specialz;
  extern uint64_t ts asm("kStartTsc");

  // check to see if the process was actually forked
  // this variable should have the pipe handle numba
  varlen = GetEnvironmentVariable(u"_FORK", fvar, ARRAYLEN(fvar));
  if (!varlen || varlen >= ARRAYLEN(fvar)) return;
  STRACE("WinMainForked()");
  SetEnvironmentVariable(u"_FORK", NULL);
#ifdef SYSDEBUG
  oncrash = AddVectoredExceptionHandler(1, NT2SYSV(OnForkCrash));
#endif
  ParseInt(ParseInt(fvar, &reader), &writer);
  CloseHandle(writer);

  // read the cpu state from the parent process & plus
  // read the list of mappings from the parent process
  // this is stored in a special secretive memory map!
  // read ExtendMemoryIntervals for further details :|
  maps = (void *)kMemtrackStart;
  if (!ReadAll(reader, jb, sizeof(jb)) ||
      !ReadAll(reader, &mapcount, sizeof(_mmi.i)) ||
      !ReadAll(reader, &mapcapacity, sizeof(_mmi.n))) {
    ExitProcess(40);
  }
  specialz = ROUNDUP(mapcapacity * sizeof(_mmi.p[0]), kMemtrackGran);
  if (!MapViewOfFileEx(CreateFileMapping(-1, 0, kNtPageReadwrite,
                                         specialz >> 32, specialz, 0),
                       kNtFileMapWrite, 0, 0, specialz, maps)) {
    ExitProcess(41);
  }
  if (!ReadAll(reader, maps, mapcount * sizeof(_mmi.p[0]))) {
    ExitProcess(42);
  }
  if (IsAsan()) {
    shad = (char *)(((intptr_t)maps >> 3) + 0x7fff8000);
    size = ROUNDUP(specialz >> 3, FRAMESIZE);
    MapViewOfFileEx(
        CreateFileMapping(-1, 0, kNtPageReadwrite, size >> 32, size, 0),
        kNtFileMapWrite, 0, 0, size, maps);
    if (!ReadAll(reader, shad, (mapcount * sizeof(_mmi.p[0])) >> 3)) {
      ExitProcess(43);
    }
  }

  // read the heap mappings from the parent process
  // we can avoid copying via pipe for shared maps!
  for (i = 0; i < mapcount; ++i) {
    addr = (char *)((uint64_t)maps[i].x << 16);
    size = maps[i].size;
    if (maps[i].flags & MAP_PRIVATE) {
      upsize = ROUNDUP(size, FRAMESIZE);
      if (maps[i].prot & PROT_EXEC) {
        flags1 = kNtPageExecuteReadwrite;
        flags2 = kNtFileMapWrite | kNtFileMapExecute;
      } else {
        flags1 = kNtPageReadwrite;
        flags2 = kNtFileMapWrite;
      }
      // we don't need to close the map handle because sys_mmap_nt
      // doesn't mark it inheritable across fork() for MAP_PRIVATE
      if (!(maps[i].h =
                CreateFileMapping(-1, 0, flags1, upsize >> 32, upsize, 0)) ||
          !MapViewOfFileEx(maps[i].h, flags2, 0, 0, upsize, addr) ||
          !ReadAll(reader, addr, size)) {
        ExitProcess(44);
      }
    } else {
      // we can however safely inherit MAP_SHARED with zero copy
      if (maps[i].prot & PROT_WRITE) {
        if (maps[i].prot & PROT_EXEC) {
          flags2 = kNtFileMapWrite | kNtFileMapExecute;
        } else {
          flags2 = kNtFileMapWrite;
        }
      } else if (maps[i].prot & PROT_READ) {
        if (maps[i].prot & PROT_EXEC) {
          flags2 = kNtFileMapRead | kNtFileMapExecute;
        } else {
          flags2 = kNtFileMapRead;
        }
      } else {
        flags2 = 0;
      }
      if (!MapViewOfFileEx(maps[i].h, flags2, maps[i].offset >> 32,
                           maps[i].offset, size, addr)) {
        ExitProcess(45);
      }
    }
  }

  // read the .data and .bss program image sections
  savepid = __pid;
  savebir = __kbirth;
  savetsc = ts;
  if (!ReadAll(reader, __data_start, __data_end - __data_start) ||
      !ReadAll(reader, __bss_start, __bss_end - __bss_start)) {
    ExitProcess(46);
  }
  __pid = savepid;
  __kbirth = savebir;
  ts = savetsc;

  // apply fixups and reapply memory protections
  _mmi.p = maps;
  _mmi.n = specialz / sizeof(_mmi.p[0]);
  for (i = 0; i < mapcount; ++i) {
    if ((maps[i].flags & MAP_PRIVATE) && (~maps[i].prot & PROT_WRITE)) {
      if (maps[i].prot & PROT_WRITE) {
        if (maps[i].prot & PROT_EXEC) {
          flags1 = kNtPageExecuteReadwrite;
        } else {
          flags1 = kNtPageReadwrite;
        }
      } else if (maps[i].prot & PROT_READ) {
        if (maps[i].prot & PROT_EXEC) {
          flags1 = kNtPageExecuteRead;
        } else {
          flags1 = kNtPageReadonly;
        }
      } else {
        flags1 = kNtPageNoaccess;
      }
      VirtualProtect((void *)((uint64_t)maps[i].x << 16),
                     ROUNDUP(maps[i].size, FRAMESIZE), flags1, &oldprot);
    }
  }

  // we're all done reading!
  if (!CloseHandle(reader)) {
    STRACE("CloseHandle(reader) failed %m");
    ExitProcess(47);
  }

  // clean up, restore state, and jump back into function below
#ifdef SYSDEBUG
  RemoveVectoredExceptionHandler(oncrash);
#endif
  if (weaken(__wincrash_nt)) {
    AddVectoredExceptionHandler(1, (void *)weaken(__wincrash_nt));
  }
  longjmp(jb, 1);
}

textwindows int sys_fork_nt(void) {
  bool ok;
  jmp_buf jb;
  char **args, **args2;
  int64_t reader, writer;
  int i, n, rc, pid, untrackpid;
  char *p, forkvar[6 + 21 + 1 + 21 + 1];
  struct NtStartupInfo startinfo;
  struct NtProcessInformation procinfo;
  if ((pid = untrackpid = __reservefd()) == -1) return -1;
  if (!setjmp(jb)) {
    if (CreatePipe(&reader, &writer, &kNtIsInheritable, 0)) {
      p = stpcpy(forkvar, "_FORK=");
      p += uint64toarray_radix10(reader, p), *p++ = ' ';
      p += uint64toarray_radix10(writer, p);
      bzero(&startinfo, sizeof(startinfo));
      startinfo.cb = sizeof(struct NtStartupInfo);
      startinfo.dwFlags = kNtStartfUsestdhandles;
      startinfo.hStdInput = g_fds.p[0].handle;
      startinfo.hStdOutput = g_fds.p[1].handle;
      startinfo.hStdError = g_fds.p[2].handle;
      args = __argv;
#ifdef SYSDEBUG
      // If --strace was passed to this program, then propagate it the
      // forked process since the flag was removed by __intercept_flag
      if (__strace > 0) {
        for (n = 0; args[n];) ++n;
        args2 = alloca((n + 2) * sizeof(char *));
        for (i = 0; i < n; ++i) args2[i] = args[i];
        args2[i++] = "--strace";
        args2[i] = 0;
        args = args2;
      }
#endif
      if (ntspawn(GetProgramExecutableName(), args, environ, forkvar,
                  &kNtIsInheritable, NULL, true, 0, NULL, &startinfo,
                  &procinfo) != -1) {
        CloseHandle(reader);
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
          if (_mmi.p[i].flags & MAP_PRIVATE) {
            ok = WriteAll(writer, (void *)((uint64_t)_mmi.p[i].x << 16),
                          _mmi.p[i].size);
          }
        }
        if (ok) ok = WriteAll(writer, __data_start, __data_end - __data_start);
        if (ok) ok = WriteAll(writer, __bss_start, __bss_end - __bss_start);
        if (ok) {
          if (!CloseHandle(writer)) {
            STRACE("CloseHandle(writer) failed %m");
            ok = false;
          }
        }
        if (ok) {
          if (!weaken(__sighandrvas) ||
              weaken(__sighandrvas)[SIGCHLD] != SIG_IGN) {
            g_fds.p[pid].kind = kFdProcess;
            g_fds.p[pid].handle = procinfo.hProcess;
            g_fds.p[pid].flags = O_CLOEXEC;
            untrackpid = -1;
            rc = pid;
          } else {
            /*
             * XXX: Ignoring SIGCHLD should track the process information.
             *      What we need to do instead, is periodically check if a
             *      process has exited and remove it automatically via i/o
             *      functions like poll() so it doesn't get zombdied.
             */
            STRACE("fork() parent closing process handle b/c SIGCHLD=SIG_IGN");
            rc = GetProcessId(procinfo.hProcess);
            CloseHandle(procinfo.hProcess);
          }
        } else {
          rc = __winerr();
          TerminateProcess(procinfo.hProcess, 127);
          CloseHandle(procinfo.hProcess);
        }
      } else {
        CloseHandle(writer);
        rc = -1;
      }
    } else {
      STRACE("CreatePipe() failed %m");
      rc = __winerr();
      CloseHandle(writer);
    }
  } else {
    rc = 0;
  }
  if (untrackpid != -1) {
    __releasefd(untrackpid);
  }
  return rc;
}
