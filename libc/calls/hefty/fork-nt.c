/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/internal.h"
#include "libc/conv/itoa.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

static textwindows int64_t ParseInt(char16_t **p) {
  uint64_t x = 0;
  while ('0' <= **p && **p <= '9') {
    x *= 10;
    x += *(*p)++ - '0';
  }
  return x;
}

static noinline textwindows void DoAll(int64_t h, void *buf, size_t n,
                                       bool32 (*f)()) {
  char *p;
  size_t i;
  uint32_t x;
  for (p = buf, i = 0; i < n; i += x) {
    f(h, p + i, n - i, &x, NULL);
  }
}

static noinline textwindows void WriteAll(int64_t h, void *buf, size_t n) {
  DoAll(h, buf, n, WriteFile);
}

static noinline textwindows void ReadAll(int64_t h, void *buf, size_t n) {
  DoAll(h, buf, n, ReadFile);
}

textwindows void WinMainForked(void) {
  int64_t h;
  void *addr;
  jmp_buf jb;
  char16_t *p;
  uint64_t size;
  char16_t var[21 + 1 + 21 + 1];
  uint32_t i, varlen, protect, access;
  varlen = GetEnvironmentVariable(u"_FORK", var, ARRAYLEN(var));
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
    switch (_mmi.p[i].prot) {
      case PROT_READ | PROT_WRITE | PROT_EXEC:
        protect = kNtPageExecuteReadwrite;
        access = kNtFileMapRead | kNtFileMapWrite | kNtFileMapExecute;
        break;
      case PROT_READ | PROT_WRITE:
        protect = kNtPageReadwrite;
        access = kNtFileMapRead | kNtFileMapWrite;
        break;
      case PROT_READ:
        protect = kNtPageReadonly;
        access = kNtFileMapRead;
        break;
      default:
        protect = kNtPageNoaccess;
        access = 0;
        break;
    }
    if (_mmi.p[i].flags & MAP_PRIVATE) {
      MapViewOfFileExNuma(
          (_mmi.p[i].h = CreateFileMappingNuma(-1, NULL, protect, 0, size, NULL,
                                               kNtNumaNoPreferredNode)),
          access, 0, 0, size, addr, kNtNumaNoPreferredNode);
      ReadAll(h, addr, size);
    } else {
      MapViewOfFileExNuma(_mmi.p[i].h, access, 0, 0, size, addr,
                          kNtNumaNoPreferredNode);
    }
  }
  ReadAll(h, _edata, _end - _edata);
  CloseHandle(h);
  longjmp(jb, 1);
}

textwindows int fork$nt(void) {
  jmp_buf jb;
  int64_t reader, writer;
  int i, rc, pid, fds[3];
  char *p, buf[21 + 1 + 21 + 1];
  if (!setjmp(jb)) {
    if (CreatePipe(&reader, &writer, &kNtIsInheritable, 0)) {
      p = buf;
      p += uint64toarray_radix10(reader, p);
      *p++ = ' ';
      p += uint64toarray_radix10(writer, p);
      setenv("_FORK", buf, true);
      fds[0] = 0;
      fds[1] = 1;
      fds[2] = 2;
      /* TODO: CloseHandle(g_fds.p[pid].h) if SIGCHLD is SIG_IGN */
      if ((pid = spawnve(0, fds, g_argv[0], g_argv, environ)) != -1) {
        CloseHandle(reader);
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
        rc = pid;
      } else {
        rc = -1;
      }
      unsetenv("_FORK");
    } else {
      rc = winerr();
    }
  } else {
    rc = 0;
  }
  return rc;
}
