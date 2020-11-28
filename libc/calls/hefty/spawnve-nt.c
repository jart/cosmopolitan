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
#include "libc/bits/xchg.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/internal.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/calls/internal.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"

textwindows int spawnve$nt(unsigned flags, int stdiofds[3], const char *program,
                           char *const argv[], char *const envp[]) {
  int pid;
  size_t i;
  int tubes[3];
  int64_t handle, h, *x, *y;
  struct NtStartupInfo sti;
  struct NtProcessInformation procinfo;

  handle = 0;
  memset(&sti, 0, sizeof(sti));
  sti.cb = sizeof(sti);
  sti.dwFlags = kNtStartfUsestdhandles;

  if ((pid = __getemptyfd()) == -1) return -1;

  for (i = 0; i < 3; ++i) {
    if (stdiofds[i] == -1) {
      x = &h;
      y = &sti.stdiofds[i];
      if (kIoMotion[i]) xchg(&x, &y);
      if ((tubes[i] = __getemptyfd()) != -1 &&
          CreatePipe(x, y, &kNtIsInheritable, 0)) {
        g_fds.p[tubes[i]].handle = h;
      } else {
        handle = -1;
      }
    } else {
      sti.stdiofds[i] = g_fds.p[stdiofds[i]].handle;
    }
  }

  if (handle != -1 &&
      ntspawn(program, argv, envp, &kNtIsInheritable, NULL,
              (flags & SPAWN_TABULARASA) ? false : true,
              (flags & SPAWN_DETACH)
                  ? (kNtCreateNewProcessGroup | kNtDetachedProcess |
                     kNtCreateBreakawayFromJob)
                  : 0,
              NULL, &sti, &procinfo) != -1) {
    CloseHandle(procinfo.hThread);
    handle = procinfo.hProcess;
  }

  for (i = 0; i < 3; ++i) {
    if (stdiofds[i] == -1) {
      if (handle != -1) {
        stdiofds[i] = tubes[i];
        g_fds.p[tubes[i]].kind = kFdFile;
        g_fds.p[tubes[i]].flags = 0;
        CloseHandle(sti.stdiofds[i]);
      } else {
        CloseHandle(tubes[i]);
      }
    }
  }

  g_fds.p[pid].kind = kFdProcess;
  g_fds.p[pid].handle = handle;
  g_fds.p[pid].flags = flags;
  return pid;
}
