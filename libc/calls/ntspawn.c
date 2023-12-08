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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/procthreadattributelist.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/startupinfoex.h"
#include "libc/proc/ntspawn.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

struct SpawnBlock {
  char16_t path[PATH_MAX];
  char16_t cmdline[32767];
  char16_t envblock[32767];
  char envbuf[32767];
};

static void *ntspawn_malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

static void ntspawn_free(void *ptr) {
  HeapFree(GetProcessHeap(), 0, ptr);
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
textwindows int ntspawn(
    int64_t dirhand, const char *prog, char *const argv[], char *const envp[],
    char *const extravars[], uint32_t dwCreationFlags,
    const char16_t *opt_lpCurrentDirectory, int64_t opt_hParentProcess,
    int64_t *opt_lpExplicitHandleList, uint32_t dwExplicitHandleCount,
    const struct NtStartupInfo *lpStartupInfo,
    struct NtProcessInformation *opt_out_lpProcessInformation) {
  int rc = -1;
  struct SpawnBlock *sb;
  BLOCK_SIGNALS;
  if ((sb = ntspawn_malloc(sizeof(*sb))) &&
      __mkntpathath(dirhand, prog, 0, sb->path) != -1) {
    if (!mkntcmdline(sb->cmdline, argv) &&
        !mkntenvblock(sb->envblock, envp, extravars, sb->envbuf)) {
      bool32 ok;
      int64_t dp = GetCurrentProcess();

      // create attribute list
      // this code won't call malloc in practice
      void *freeme = 0;
      _Alignas(16) char memory[128];
      size_t size = sizeof(memory);
      struct NtProcThreadAttributeList *alist = (void *)memory;
      uint32_t items = !!opt_hParentProcess + !!opt_lpExplicitHandleList;
      ok = InitializeProcThreadAttributeList(alist, items, 0, &size);
      if (!ok && GetLastError() == kNtErrorInsufficientBuffer) {
        ok = !!(alist = freeme = ntspawn_malloc(size));
        if (ok) {
          ok = InitializeProcThreadAttributeList(alist, items, 0, &size);
        }
      }
      if (ok && opt_hParentProcess) {
        ok = UpdateProcThreadAttribute(
            alist, 0, kNtProcThreadAttributeParentProcess, &opt_hParentProcess,
            sizeof(opt_hParentProcess), 0, 0);
      }
      if (ok && opt_lpExplicitHandleList) {
        ok = UpdateProcThreadAttribute(
            alist, 0, kNtProcThreadAttributeHandleList,
            opt_lpExplicitHandleList,
            dwExplicitHandleCount * sizeof(*opt_lpExplicitHandleList), 0, 0);
      }

      // create the process
      if (ok) {
        struct NtStartupInfoEx info;
        bzero(&info, sizeof(info));
        info.StartupInfo = *lpStartupInfo;
        info.StartupInfo.cb = sizeof(info);
        info.lpAttributeList = alist;
        if (ok) {
          if (CreateProcess(sb->path, sb->cmdline, 0, 0, true,
                            dwCreationFlags | kNtCreateUnicodeEnvironment |
                                kNtExtendedStartupinfoPresent |
                                kNtInheritParentAffinity |
                                GetPriorityClass(GetCurrentProcess()),
                            sb->envblock, opt_lpCurrentDirectory,
                            &info.StartupInfo, opt_out_lpProcessInformation)) {
            rc = 0;
          } else {
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
      if (alist) {
        DeleteProcThreadAttributeList(alist);
      }
      if (freeme) {
        ntspawn_free(freeme);
      }
      if (dp && dp != GetCurrentProcess()) {
        CloseHandle(dp);
      }
    }
  }
  if (sb) ntspawn_free(sb);
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
