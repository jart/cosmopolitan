/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/th32cs.h"
#include "libc/nt/errors.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processentry32.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static inline int GetConsoleCtrlEvent(int sig) {
  switch (sig) {
    case SIGINT:
      return kNtCtrlCEvent;
    case SIGQUIT:
      return kNtCtrlBreakEvent;
    default:
      return -1;
  }
}

textwindows int sys_kill_nt(int pid, int sig) {
  bool32 ok;
  int64_t h;
  int event, ntpid;

  // is killing everything except init really worth supporting?
  if (pid == -1) return einval();

  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  pid = ABS(pid);

  // If we're targeting current process group then just call raise().
  if (!pid || pid == getpid()) {
    if (!sig) return 0;  // ability check passes
    return raise(sig);
  }

  // GenerateConsoleCtrlEvent() will always signal groups and there's
  // nothing we can do about it, unless we have a GUI GetMessage loop
  // and alternatively create a centralized signal daemon like cygwin
  if ((event = GetConsoleCtrlEvent(sig)) != -1) {
    // we're killing with SIGINT or SIGQUIT which are the only two
    // signals we can really use, since TerminateProcess() makes
    // everything else effectively a SIGKILL ;_;
    if (__isfdkind(pid, kFdProcess)) {
      ntpid = GetProcessId(g_fds.p[pid].handle);
    } else if (!__isfdopen(pid)) {
      ntpid = pid;  // XXX: suboptimal
    } else {
      return esrch();
    }
    if (GenerateConsoleCtrlEvent(event, ntpid)) {
      return 0;
    } else {
      return -1;
    }
  }

  // is this a cosmo pid that was returned by fork?
  if (__isfdkind(pid, kFdProcess)) {
    if (!sig) return 0;  // ability check passes
    // since windows can't execve we need to kill the grandchildren
    // TODO(jart): should we just kill the whole tree too? there's
    //             no obvious way to tell if it's the execve shell
    struct NtProcessEntry32 pe = {.dwSize = sizeof(struct NtProcessEntry32)};
    ntpid = GetProcessId(g_fds.p[pid].handle);
    int64_t hSnap = CreateToolhelp32Snapshot(kNtTh32csSnapprocess, 0);
    if (Process32First(hSnap, &pe)) {
      do {
        if (pe.th32ParentProcessID == ntpid) {
          if ((h = OpenProcess(kNtProcessTerminate, false, pe.th32ProcessID))) {
            TerminateProcess(h, sig);
            CloseHandle(h);
          }
        }
      } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    ok = TerminateProcess(g_fds.p[pid].handle, sig);
    if (!ok && GetLastError() == kNtErrorAccessDenied) ok = true;
    return 0;
  }

  // XXX: Is this a raw new technology pid? Because that's messy.
  if ((h = OpenProcess(kNtProcessTerminate, false, pid))) {
    if (sig) {
      ok = TerminateProcess(h, sig);
      if (!ok && GetLastError() == kNtErrorAccessDenied) {
        ok = true;  // cargo culting other codebases here
      }
    }
    CloseHandle(h);
    return 0;
  } else {
    return -1;
  }
}
