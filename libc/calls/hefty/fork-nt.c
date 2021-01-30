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
#include "libc/fmt/itoa.h"
#include "libc/macros.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/filemapflags.h"
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
#include "libc/runtime/directmap.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static textwindows int64_t ParseInt(char16_t **p) {
  uint64_t x = 0;
  while ('0' <= **p && **p <= '9') {
    x *= 10;
    x += *(*p)++ - '0';
  }
  return x;
}

static noinline textwindows void ForkIo(int64_t h, void *buf, size_t n,
                                        bool32 (*f)()) {
  char *p;
  size_t i;
  uint32_t x;
  for (p = buf, i = 0; i < n; i += x) {
    f(h, p + i, n - i, &x, NULL);
  }
}

static noinline textwindows void WriteAll(int64_t h, void *buf, size_t n) {
  ForkIo(h, buf, n, WriteFile);
}

static noinline textwindows void ReadAll(int64_t h, void *buf, size_t n) {
  ForkIo(h, buf, n, ReadFile);
}

textwindows void WinMainForked(void) {
  int64_t h;
  void *addr;
  jmp_buf jb;
  char16_t *p;
  uint64_t size;
  uint32_t i, varlen;
  struct DirectMap dm;
  char16_t var[21 + 1 + 21 + 1];
  varlen = GetEnvironmentVariable(u"_FORK", var, ARRAYLEN(var));
  SetEnvironmentVariable(u"_FORK", NULL);
  if (!varlen || varlen >= ARRAYLEN(var)) return;
  p = var;
  h = ParseInt(&p);
  if (*p++ == ' ') CloseHandle(ParseInt(&p));
  ReadAll(h, jb, sizeof(jb));
  ReadAll(h, &_mmi.i, sizeof(_mmi.i));
  for (i = 0; i < _mmi.i; ++i) {
    ReadAll(h, &_mmi.p[i], sizeof(_mmi.p[i]));
    addr = (void *)((uint64_t)_mmi.p[i].x << 16);
    size = ((uint64_t)(_mmi.p[i].y - _mmi.p[i].x) << 16) + FRAMESIZE;
    if (_mmi.p[i].flags & MAP_PRIVATE) {
      CloseHandle(_mmi.p[i].h);
      _mmi.p[i].h =
          __mmap$nt(addr, size, PROT_READ | PROT_WRITE | PROT_EXEC, -1, 0)
              .maphandle;
      ReadAll(h, addr, size);
    } else {
      MapViewOfFileExNuma(
          _mmi.p[i].h,
          (_mmi.p[i].prot & PROT_WRITE)
              ? kNtFileMapWrite | kNtFileMapExecute | kNtFileMapRead
              : kNtFileMapExecute | kNtFileMapRead,
          0, 0, size, addr, kNtNumaNoPreferredNode);
    }
  }
  ReadAll(h, _edata, _end - _edata);
  CloseHandle(h);
  unsetenv("_FORK");
  if (weaken(__wincrash$nt)) {
    AddVectoredExceptionHandler(1, (void *)weaken(__wincrash$nt));
  }
  longjmp(jb, 1);
}

textwindows int fork$nt(void) {
  jmp_buf jb;
  int i, rc, pid;
  char exe[PATH_MAX];
  int64_t reader, writer;
  char *p, buf[21 + 1 + 21 + 1];
  struct NtStartupInfo startinfo;
  struct NtProcessInformation procinfo;
  if ((pid = __getemptyfd()) == -1) return -1;
  if (!setjmp(jb)) {
    if (CreatePipe(&reader, &writer, &kNtIsInheritable, 0)) {
      p = buf;
      p += uint64toarray_radix10(reader, p);
      *p++ = ' ';
      p += uint64toarray_radix10(writer, p);
      setenv("_FORK", buf, true);
      memset(&startinfo, 0, sizeof(startinfo));
      startinfo.cb = sizeof(struct NtStartupInfo);
      startinfo.dwFlags = kNtStartfUsestdhandles;
      startinfo.hStdInput = g_fds.p[0].handle;
      startinfo.hStdOutput = g_fds.p[1].handle;
      startinfo.hStdError = g_fds.p[2].handle;
      GetModuleFileNameA(0, exe, ARRAYLEN(exe));
      if (ntspawn(exe, g_argv, environ, &kNtIsInheritable, NULL, true, 0, NULL,
                  &startinfo, &procinfo) != -1) {
        CloseHandle(reader);
        CloseHandle(procinfo.hThread);
        if (weaken(__sighandrvas) &&
            weaken(__sighandrvas)[SIGCHLD] == SIG_IGN) {
          CloseHandle(procinfo.hProcess);
        } else {
          g_fds.p[pid].kind = kFdProcess;
          g_fds.p[pid].handle = procinfo.hProcess;
          g_fds.p[pid].flags = O_CLOEXEC;
        }
        WriteAll(writer, jb, sizeof(jb));
        WriteAll(writer, &_mmi.i, sizeof(_mmi.i));
        for (i = 0; i < _mmi.i; ++i) {
          WriteAll(writer, &_mmi.p[i], sizeof(_mmi.p[i]));
          if (_mmi.p[i].flags & MAP_PRIVATE) {
            WriteAll(writer, (void *)((uint64_t)_mmi.p[i].x << 16),
                     ((uint64_t)(_mmi.p[i].y - _mmi.p[i].x) << 16) + FRAMESIZE);
          }
        }
        WriteAll(writer, _edata, _end - _edata);
        CloseHandle(writer);
      } else {
        rc = -1;
      }
      unsetenv("_FORK");
      rc = pid;
    } else {
      rc = __winerr();
    }
  } else {
    rc = 0;
  }
  return rc;
}
