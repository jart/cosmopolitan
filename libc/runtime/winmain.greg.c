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
#include "libc/bits/pushpop.h"
#include "libc/dce.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/loadlibrarysearch.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/getdosenviron.h"
#include "libc/runtime/internal.h"

static void LoadFasterAndPreventHijacking(void) {
  unsigned wrote;
  if (!SetDefaultDllDirectories(kNtLoadLibrarySearchSearchSystem32)) {
    WriteFile(GetStdHandle(kNtStdErrorHandle), "nodll\n", 6, &wrote, NULL);
    ExitProcess(1);
  }
}

noreturn textwindows int WinMain(void *hInstance, void *hPrevInstance,
                                 const char *lpCmdLine, int nCmdShow) {
  int i, count;
  const char16_t *cmd16, *env16;
  long auxarray[][2] = {{pushpop(0L), pushpop(0L)}};
  char envblock[ENV_MAX], *envarray[512], argblock[ARG_MAX], *argarray[512];
  LoadFasterAndPreventHijacking();
  *(/*unconst*/ int *)&__hostos = pushpop(WINDOWS);
  cmd16 = GetCommandLine();
  env16 = GetEnvironmentStrings();
  count = getdosargv(cmd16, argblock, ARG_MAX, argarray, 512);
  for (i = 0; argarray[0][i]; ++i) {
    if (argarray[0][i] == '\\') argarray[0][i] = '/';
  }
  getdosenviron(env16, envblock, ENV_MAX, envarray, 512);
  FreeEnvironmentStrings(env16);
  register int argc asm("r12") = count;
  register char **argv asm("r13") = argarray;
  register char **envp asm("r14") = envarray;
  register long(*auxv)[2] asm("r15") = auxarray;
  asm volatile("jmp\t__executive"
               : /* no outputs */
               : "r"(argc), "r"(argv), "r"(envp), "r"(auxv)
               : "memory", "cc");
  unreachable;
}
