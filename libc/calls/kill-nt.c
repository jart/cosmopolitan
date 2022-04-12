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
#include "libc/calls/getconsolectrlevent.internal.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/errors.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_kill_nt(int pid, int sig) {
  bool32 ok;
  int64_t handle;
  int event, ntpid;

  // is killing everything except init really worth supporting?
  if (pid == -1) return einval();

  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  pid = ABS(pid);

  // If we're targeting current process group then just call raise().
  if (!pid || pid == getpid()) {
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

  // XXX: Is this a cosmo pid that was returned by fork_nt?
  if (__isfdkind(pid, kFdProcess)) {
    ok = TerminateProcess(g_fds.p[pid].handle, 128 + sig);
    if (!ok && GetLastError() == kNtErrorAccessDenied) ok = true;
    return 0;
  }

  // XXX: Is this a raw new technology pid? Because that's messy.
  if ((handle = OpenProcess(kNtProcessTerminate, false, pid))) {
    ok = TerminateProcess(handle, 128 + sig);
    if (!ok && GetLastError() == kNtErrorAccessDenied) {
      ok = true;  // cargo culting other codebases here
    }
    CloseHandle(handle);
    return 0;
  } else {
    return -1;
  }
}
