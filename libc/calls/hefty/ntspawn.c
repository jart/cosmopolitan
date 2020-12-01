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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.internal.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/calls/internal.h"
#include "libc/conv/conv.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"

/**
 * Spawns process on Windows NT.
 *
 * This function delegates to CreateProcess() with UTF-8 → UTF-16
 * translation and argv escaping. Please note this will NOT escape
 * command interpreter syntax.
 *
 * @param program will not be PATH searched, see commandv()
 * @param argv[0] is the name of the program to run
 * @param argv[1,n-2] optionally specifies program arguments
 * @param argv[n-1] is NULL
 * @param envp[𝟶,m-2] specifies "foo=bar" environment variables, which
 *     don't need to be passed in sorted order; however, this function
 *     goes faster the closer they are to sorted
 * @param envp[m-1] is NULL
 * @param bInheritHandles means handles already marked inheritable will
 *     be inherited; which, assuming the System V wrapper functions are
 *     being used, should mean (1) all files and sockets that weren't
 *     opened with O_CLOEXEC; and (2) all memory mappings
 * @param opt_out_lpProcessInformation can be used to return process and
 *     thread IDs to parent, as well as open handles that need close()
 * @return 0 on success, or -1 w/ errno
 * @see spawnve() which abstracts this function
 */
textwindows int ntspawn(
    const char *program, char *const argv[], char *const envp[],
    struct NtSecurityAttributes *opt_lpProcessAttributes,
    struct NtSecurityAttributes *opt_lpThreadAttributes, bool32 bInheritHandles,
    uint32_t dwCreationFlags, const char16_t *opt_lpCurrentDirectory,
    /*nodiscard*/ const struct NtStartupInfo *lpStartupInfo,
    struct NtProcessInformation *opt_out_lpProcessInformation) {
  int rc;
  char16_t program16[PATH_MAX], *lpCommandLine, *lpEnvironment;
  lpCommandLine = NULL;
  lpEnvironment = NULL;
  if (__mkntpath(program, program16) != -1 &&
      (lpCommandLine = mkntcmdline(&argv[1])) &&
      (lpEnvironment = mkntenvblock(envp))) {
    if (CreateProcess(program16, lpCommandLine, opt_lpProcessAttributes,
                      opt_lpThreadAttributes, bInheritHandles,
                      dwCreationFlags | kNtCreateUnicodeEnvironment,
                      lpEnvironment, opt_lpCurrentDirectory, lpStartupInfo,
                      opt_out_lpProcessInformation)) {
      rc = 0;
    } else {
      rc = __winerr();
    }
  } else {
    rc = -1;
  }
  free(lpCommandLine);
  free(lpEnvironment);
  return rc;
}
