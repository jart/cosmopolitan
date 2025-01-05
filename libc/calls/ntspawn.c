/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/proc/ntspawn.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/procthreadattributelist.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/startupinfoex.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/proc/ntspawn.h"
#include "libc/stdalign.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;

struct SpawnBlock {
  char16_t path[PATH_MAX];
  char16_t cmdline[32767];
  char16_t envblock[32767];
  char envbuf[32767];
};

static textwindows void *ntspawn_malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

static textwindows void ntspawn_free(void *ptr) {
  HeapFree(GetProcessHeap(), 0, ptr);
}

static textwindows ssize_t ntspawn_read(intptr_t fh, char *buf, size_t len) {
  bool ok;
  uint32_t got;
  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 0, 0, 0)};
  ok = overlap.hEvent &&
       (ReadFile(fh, buf, len, 0, &overlap) ||
        GetLastError() == kNtErrorIoPending) &&
       GetOverlappedResult(fh, &overlap, &got, true);
  if (overlap.hEvent)
    __imp_CloseHandle(overlap.hEvent);
  return ok ? got : -1;
}

static textwindows int ntspawn2(struct NtSpawnArgs *a, struct SpawnBlock *sb) {

  // make executable path
  if (__mkntpathath(a->dirhand, a->prog, 0, sb->path) == -1)
    return -1;

  // open executable
  char *p = sb->envbuf;
  char *pe = p + sizeof(sb->envbuf);
  intptr_t fh = CreateFile(
      sb->path, kNtFileGenericRead,
      kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
      kNtOpenExisting, kNtFileAttributeNormal | kNtFileFlagBackupSemantics, 0);
  if (fh == -1)
    return -1;
  ssize_t got = ntspawn_read(fh, p, pe - p);
  __imp_CloseHandle(fh);
  if (got < 3)
    return enoexec();
  pe = p + got;

  // handle shebang
  size_t i = 0;  // represents space of sb->cmdline consumed
  if (p[0] == 'M' && p[1] == 'Z') {
    // it's a windows executable
  } else if (p[0] == '#' && p[1] == '!') {
    p += 2;
    // make sure we got a complete first line
    pe = memchr(p, '\n', pe - p);
    if (!pe)
      return enoexec();
    *pe = 0;
    int argc = 0;
    char *argv[4];
    // it's legal to say "#! /bin/sh"
    while (p < pe && (*p == ' ' || *p == '\t'))
      ++p;
    if (p == pe)
      return enoexec();
    argv[argc++] = p;
    // find the optional argument
    while (p < pe && !(*p == ' ' || *p == '\t'))
      ++p;
    if (p < pe) {
      *p++ = 0;
      while (p < pe && (*p == ' ' || *p == '\t'))
        ++p;
      if (p < pe) {
        argv[argc++] = p;
      }
    }
    // now add the prog
    argv[argc++] = (char *)a->prog;
    argv[argc] = 0;
    // ignore argv[0]
    if (*a->argv)
      ++a->argv;
    // prepend arguments
    if ((i += mkntcmdline(sb->cmdline + i, argv, 32767 - i)) >= 32767 - 1)
      return e2big();
    sb->cmdline[i++] = ' ';
    sb->cmdline[i] = 0;
    // setup the true executable path
    if (__mkntpathath(a->dirhand, argv[0], 0, sb->path) == -1)
      return -1;
  } else {
    // it's something else
    return enoexec();
  }

  // setup arguments and environment
  if ((i += mkntcmdline(sb->cmdline + i, a->argv, 32767 - i)) >= 32767)
    return e2big();
  if (mkntenvblock(sb->envblock, a->envp, a->extravars, sb->envbuf) == -1)
    return -1;

  // create attribute list
  // this code won't call malloc in practice
  bool32 ok;
  void *freeme = 0;
  alignas(16) char memory[128];
  size_t size = sizeof(memory);
  struct NtProcThreadAttributeList *alist = (void *)memory;
  uint32_t items = !!a->opt_hParentProcess + !!a->dwExplicitHandleCount;
  ok = InitializeProcThreadAttributeList(alist, items, 0, &size);
  if (!ok && GetLastError() == kNtErrorInsufficientBuffer) {
    ok = !!(alist = freeme = ntspawn_malloc(size));
    if (ok) {
      ok = InitializeProcThreadAttributeList(alist, items, 0, &size);
    }
  }
  if (ok && a->opt_hParentProcess) {
    ok = UpdateProcThreadAttribute(
        alist, 0, kNtProcThreadAttributeParentProcess, &a->opt_hParentProcess,
        sizeof(a->opt_hParentProcess), 0, 0);
  }
  if (ok && a->dwExplicitHandleCount) {
    ok = UpdateProcThreadAttribute(
        alist, 0, kNtProcThreadAttributeHandleList, a->opt_lpExplicitHandleList,
        a->dwExplicitHandleCount * sizeof(*a->opt_lpExplicitHandleList), 0, 0);
  }

  // create the process
  int rc;
  if (ok) {
    struct NtStartupInfoEx info = {
        .StartupInfo = *a->lpStartupInfo,
        .lpAttributeList = alist,
    };
    info.StartupInfo.cb = sizeof(info);
    if (ok) {
      if (CreateProcess(sb->path, sb->cmdline, 0, 0, true,
                        a->dwCreationFlags | kNtCreateUnicodeEnvironment |
                            kNtExtendedStartupinfoPresent |
                            kNtInheritParentAffinity |
                            GetPriorityClass(GetCurrentProcess()),
                        sb->envblock, a->opt_lpCurrentDirectory,
                        &info.StartupInfo, a->opt_out_lpProcessInformation)) {
        rc = 0;
      } else {
        rc = -1;
        STRACE("CreateProcess() failed w/ %d", GetLastError());
        if (GetLastError() == kNtErrorSharingViolation) {
          etxtbsy();
        } else if (GetLastError() == kNtErrorInvalidName) {
          enoent();
        }
      }
      rc = __fix_enotdir(rc, sb->path);
    }
  } else {
    rc = __winerr();
  }

  // clean up resources
  if (alist)
    DeleteProcThreadAttributeList(alist);
  if (freeme)
    ntspawn_free(freeme);
  return rc;
}

/**
 * Spawns process on Windows NT.
 *
 * This function delegates to CreateProcess() with UTF-8 → UTF-16
 * translation and argv escaping. Please note this will NOT escape
 * command interpreter syntax.
 *
 * @param prog won't be PATH searched
 * @param argv specifies prog arguments
 * @param envp[𝟶,m-2] specifies "foo=bar" environment variables, which
 *     don't need to be passed in sorted order; however, this function
 *     goes faster the closer they are to sorted
 * @param envp[m-1] is NULL
 * @param extravars is added to envp to avoid setenv() in caller
 * @param opt_out_lpProcessInformation can be used to return process and
 *     thread IDs to parent, as well as open handles that need close()
 * @return 0 on success, or -1 w/ errno
 * @see spawnve() which abstracts this function
 * @asyncsignalsafe
 */
textwindows int ntspawn(struct NtSpawnArgs *args) {
  int rc;
  struct SpawnBlock *sb;
  BLOCK_SIGNALS;
  if ((sb = ntspawn_malloc(sizeof(*sb)))) {
    rc = ntspawn2(args, sb);
    ntspawn_free(sb);
  } else {
    rc = -1;
  }
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
