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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/ntspawn.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/enum/wt.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static textwindows noasan char16_t *ParseInt(char16_t *p, int64_t *x) {
  *x = 0;
  while (*p == ' ') p++;
  while ('0' <= *p && *p <= '9') {
    *x *= 10;
    *x += *p++ - '0';
  }
  return p;
}

static dontinline textwindows noasan bool ForkIo(int64_t h, void *buf, size_t n,
                                               bool32 (*f)()) {
  char *p;
  size_t i;
  uint32_t x;
  for (p = buf, i = 0; i < n; i += x) {
    if (!f(h, p + i, n - i, &x, NULL)) {
      return false;
    }
  }
  return true;
}

static dontinline textwindows noasan void WriteAll(int64_t h, void *buf,
                                                 size_t n) {
  if (!ForkIo(h, buf, n, WriteFile)) {
    SYSDEBUG("fork() WriteFile(%zu) failed %d\n", n, GetLastError());
  }
}

static textwindows dontinline noasan void ReadAll(int64_t h, void *buf,
                                                size_t n) {
  if (!ForkIo(h, buf, n, ReadFile)) {
    SYSDEBUG("fork() ReadFile(%zu) failed %d\n", n, GetLastError());
  }
}

textwindows noasan noinstrument void WinMainForked(void) {
  void *addr;
  jmp_buf jb;
  long mapcount;
  uint64_t size;
  uint32_t i, varlen;
  struct DirectMap dm;
  int64_t reader, writer;
  struct MemoryInterval *maps;
  char16_t var[21 + 1 + 21 + 1];
  varlen = GetEnvironmentVariable(u"_FORK", var, ARRAYLEN(var));
  if (!varlen) return;
  if (varlen >= ARRAYLEN(var)) ExitProcess(123);
  SetEnvironmentVariable(u"_FORK", NULL);
  ParseInt(ParseInt(var, &reader), &writer);
  ReadAll(reader, jb, sizeof(jb));
  ReadAll(reader, &mapcount, sizeof(_mmi.i));
  maps = GlobalAlloc(0, mapcount * sizeof(*_mmi.p));
  ReadAll(reader, maps, mapcount * sizeof(*_mmi.p));
  for (i = 0; i < mapcount; ++i) {
    addr = (void *)((uint64_t)maps[i].x << 16);
    size = ((uint64_t)(maps[i].y - maps[i].x) << 16) + FRAMESIZE;
    if (maps[i].flags & MAP_PRIVATE) {
      CloseHandle(maps[i].h);
      maps[i].h =
          sys_mmap_nt(addr, size, maps[i].prot, maps[i].flags, -1, 0).maphandle;
      ReadAll(reader, addr, size);
    } else {
      MapViewOfFileExNuma(
          maps[i].h,
          (maps[i].prot & PROT_WRITE)
              ? kNtFileMapWrite | kNtFileMapExecute | kNtFileMapRead
              : kNtFileMapExecute | kNtFileMapRead,
          0, 0, size, addr, kNtNumaNoPreferredNode);
    }
  }
  ReadAll(reader, _etext, _end - _etext);
  for (i = 0; i < mapcount; ++i) {
    _mmi.p[i].h = maps[i].h;
  }
  CloseHandle(reader);
  CloseHandle(writer);
  GlobalFree(maps);
  if (weaken(__wincrash_nt)) {
    AddVectoredExceptionHandler(1, (void *)weaken(__wincrash_nt));
  }
  longjmp(jb, 1);
}

textwindows int sys_fork_nt(void) {
  jmp_buf jb;
  int64_t reader, writer;
  int i, rc, pid, releaseme;
  char *p, forkvar[6 + 21 + 1 + 21 + 1];
  struct NtStartupInfo startinfo;
  struct NtProcessInformation procinfo;
  if ((pid = releaseme = __reservefd()) == -1) return -1;
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
      if (ntspawn(program_executable_name, __argv, environ, forkvar,
                  &kNtIsInheritable, NULL, true, 0, NULL, &startinfo,
                  &procinfo) != -1) {
        CloseHandle(reader);
        CloseHandle(procinfo.hThread);
        if (weaken(__sighandrvas) &&
            weaken(__sighandrvas)[SIGCHLD] == SIG_IGN) {
          CloseHandle(procinfo.hProcess);
        } else {
          g_fds.p[pid].kind = kFdProcess;
          g_fds.p[pid].handle = procinfo.hProcess;
          g_fds.p[pid].flags = O_CLOEXEC;
          releaseme = -1;
        }
        WriteAll(writer, jb, sizeof(jb));
        WriteAll(writer, &_mmi.i, sizeof(_mmi.i));
        WriteAll(writer, _mmi.p, _mmi.i * sizeof(*_mmi.p));
        for (i = 0; i < _mmi.i; ++i) {
          if (_mmi.p[i].flags & MAP_PRIVATE) {
            WriteAll(writer, (void *)((uint64_t)_mmi.p[i].x << 16),
                     ((uint64_t)(_mmi.p[i].y - _mmi.p[i].x) << 16) + FRAMESIZE);
          }
        }
        WriteAll(writer, _etext, _end - _etext);
        CloseHandle(writer);
      } else {
        rc = -1;
      }
      rc = pid;
    } else {
      rc = __winerr();
    }
  } else {
    rc = 0;
  }
  if (releaseme != -1) {
    __releasefd(releaseme);
  }
  return rc;
}
