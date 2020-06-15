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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/escape/escape.h"
#include "libc/mem/mem.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/files.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"

#define SHELL_BIN "/bin/sh"
#define SHELL_ARG "-c"
#define CMD_C     "CMD /C "

static int system$sysv(const char *cmdline) {
  if (cmdline != NULL) {
    int pid = fork$sysv();
    if (pid == 0) {
      struct mem {
        char shell[sizeof(SHELL_BIN)];
        char arg[sizeof(SHELL_ARG)];
        char *args[4];
        char cmdline[];
      } *mem = malloc(sizeof(struct mem) + (strlen(cmdline) + 1));
      if (!mem) return enomem();
      strcpy(mem->shell, SHELL_BIN);
      strcpy(mem->arg, SHELL_ARG);
      mem->args[0] = mem->shell;
      mem->args[1] = mem->arg;
      mem->args[2] = mem->cmdline;
      mem->args[3] = NULL;
      strcpy(mem->cmdline, cmdline);
      execve$sysv(mem->shell, mem->args, environ);
      abort();
    }
    int rc;
    int wstatus;
    if ((rc = wait4$sysv(pid, &wstatus, 0, NULL)) != -1) rc = wstatus;
    return rc;
  } else {
    return fileexists(SHELL_BIN);
  }
}

static textwindows noinline int system$nt(const char *cmdline) {
  if (cmdline != NULL) {
    int rc = -1;
    unsigned dosquotemultiplier = 2;
    unsigned len = strlen(cmdline);
    unsigned cmdline16bytes = (len + 1) * sizeof(uint16_t);
    unsigned quotedcmdline16bytes =
        strlen(CMD_C) * sizeof(uint16_t) + cmdline16bytes * dosquotemultiplier;
    void *mem = malloc(sizeof(struct NtProcessInformation) + cmdline16bytes +
                       quotedcmdline16bytes);
    if (mem == NULL) return enomem();
    struct NtProcessInformation *info = mem;
    uint16_t *cmdline16 =
        (uint16_t *)((char *)mem + sizeof(struct NtProcessInformation));
    uint16_t *quotedcmdline16 =
        (uint16_t *)((char *)mem + sizeof(struct NtProcessInformation) +
                     cmdline16bytes);
    strcpyzbw(cmdline16, cmdline);
    strcpyzbw(quotedcmdline16, CMD_C);
    if (escapedos(quotedcmdline16 + strlen(CMD_C), len * dosquotemultiplier,
                  cmdline16, len)) {
      struct NtStartupInfo startinfo;
      memset(&startinfo, 0, sizeof(startinfo));
      startinfo.cb = sizeof(struct NtStartupInfo);
      startinfo.dwFlags = kNtStartfUsestdhandles;
      startinfo.hStdInput = STDIN_FILENO;
      startinfo.hStdOutput = STDOUT_FILENO;
      startinfo.hStdError = STDERR_FILENO;
      if (CreateProcess(
              /* lpApplicationName    */ NULL,
              /* lpCommandLine        */ quotedcmdline16,
              /* lpProcessAttributes  */ NULL,
              /* lpThreadAttributes   */ NULL,
              /* bInheritHandles      */ true,
              /* dwCreationFlags      */ kNtCreateNoWindow,
              /* lpEnvironment        */ NULL,
              /* lpCurrentDirectory   */ NULL,
              /* lpStartupInfo        */ &startinfo,
              /* lpProcessInformation */ info)) {
        uint32_t dwExitCode = kNtStillActive;
        int status;
        do {
          WaitForSingleObject(info->hProcess, 0xffffffff);
        } while ((status = GetExitCodeProcess(info->hProcess, &dwExitCode)) &&
                 dwExitCode == kNtStillActive);
        if (weaken(fflush)) {
          weaken(fflush)(*weaken(stderr));
        }
        rc = (dwExitCode & 0xff) << 8; /* @see WEXITSTATUS() */
        CloseHandle(info->hProcess);
        CloseHandle(info->hThread);
      } else {
        rc = winerr();
      }
    } else {
      rc = einval();
    }
    free(mem), mem = NULL;
    return rc;
  } else {
    /* how could cmd.exe not exist? */
    return true;
  }
}

/**
 * Launches program with system command interpreter.
 *
 * @param cmdline is an interpreted Turing-complete command
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s)
 */
int system(const char *cmdline) {
  int rc;
  if (weaken(fflush)) weaken(fflush)(NULL);
  if (!IsWindows()) {
    rc = system$sysv(cmdline);
  } else {
    rc = system$nt(cmdline);
  }
  return rc;
}
