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
#include "libc/bits/weaken.h"
#include "libc/dce.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/loadlibrarysearch.h"
#include "libc/nt/files.h"
#include "libc/nt/pedef.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/getdosenviron.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"

static struct CmdExe {
  bool result;
  struct OldNtConsole {
    uint32_t codepage;
    uint32_t mode;
    int64_t handle;
  } oldin, oldout;
} g_cmdexe;

static textwindows void MitigateDriveByDownloads(void) {
  unsigned wrote;
  if (!SetDefaultDllDirectories(kNtLoadLibrarySearchSearchSystem32)) {
    WriteFile(GetStdHandle(kNtStdErrorHandle), "nodll\n", 6, &wrote, NULL);
    ExitProcess(1);
  }
}

static textwindows void RestoreCmdExe(void) {
  if (g_cmdexe.oldin.handle) {
    SetConsoleCP(g_cmdexe.oldin.codepage);
    SetConsoleMode(g_cmdexe.oldin.handle, g_cmdexe.oldin.mode);
  }
  if (g_cmdexe.oldout.handle) {
    SetConsoleOutputCP(g_cmdexe.oldout.codepage);
    SetConsoleMode(g_cmdexe.oldout.handle, g_cmdexe.oldout.mode);
  }
}

static textwindows void SetTrueColor(void) {
  SetEnvironmentVariable(u"TERM", u"xterm-truecolor");
}

static textwindows void NormalizeCmdExe(void) {
  int64_t handle, hstdin, hstdout, hstderr;
  if ((int)weakaddr("v_ntsubsystem") == kNtImageSubsystemWindowsCui &&
      NtGetVersion() >= kNtVersionWindows10) {
    atexit(RestoreCmdExe);
    hstdin = GetStdHandle(pushpop(kNtStdInputHandle));
    hstdout = GetStdHandle(pushpop(kNtStdOutputHandle));
    hstderr = GetStdHandle(pushpop(kNtStdErrorHandle));
    if (GetFileType((handle = hstdin)) == kNtFileTypeChar) {
      SetTrueColor();
      g_cmdexe.oldin.handle = handle;
      g_cmdexe.oldin.codepage = GetConsoleCP();
      SetConsoleCP(kNtCpUtf8);
      GetConsoleMode(handle, &g_cmdexe.oldin.mode);
      SetConsoleMode(handle, g_cmdexe.oldin.mode | kNtEnableProcessedInput |
                                 kNtEnableEchoInput | kNtEnableLineInput |
                                 kNtEnableWindowInput |
                                 kNtEnableVirtualTerminalInput);
    }
    if (GetFileType((handle = hstdout)) == kNtFileTypeChar ||
        GetFileType((handle = hstderr)) == kNtFileTypeChar) {
      SetTrueColor();
      g_cmdexe.oldout.handle = handle;
      g_cmdexe.oldout.codepage = GetConsoleOutputCP();
      SetConsoleOutputCP(kNtCpUtf8);
      GetConsoleMode(handle, &g_cmdexe.oldout.mode);
      SetConsoleMode(handle, g_cmdexe.oldout.mode | kNtEnableProcessedOutput |
                                 kNtEnableWrapAtEolOutput |
                                 (NtGetVersion() >= kNtVersionWindows10
                                      ? kNtEnableVirtualTerminalProcessing
                                      : 0));
    }
  }
}

/**
 * Main function on Windows NT.
 *
 * The Cosmopolitan Runtime provides the following services, which aim
 * to bring Windows NT behavior closer in harmony with System Five:
 *
 *   1. We configure CMD.EXE for UTF-8 and enable ANSI colors on Win10.
 *
 *   2. Command line arguments are passed as a blob of UTF-16 text. We
 *      chop them up into an char *argv[] UTF-8 data structure, in
 *      accordance with the DOS conventions for argument quoting.
 *
 *   3. Environment variables are passed to us as a sorted UTF-16 double
 *      NUL terminated list. We translate this to char ** using UTF-8.
 *
 *   4. NT likes to choose a stack address that's beneath the program
 *      image. We want to be able to assume that stack addresses are
 *      located at higher addresses than heap and program memory. So the
 *      _executive() function will switch stacks appropriately.
 *
 *   5. Windows users are afraid of "drive-by downloads" where someone
 *      might accidentally an evil DLL to their Downloads folder which
 *      then overrides the behavior of a legitimate EXE being run from
 *      the downloads folder. Since we don't even use dynamic linking,
 *      we've cargo culted some API calls, that may harden against it.
 *
 */
textwindows int WinMain(void *hInstance, void *hPrevInstance,
                        const char *lpCmdLine, int nCmdShow) {
  int i, count;
  const char16_t *cmd16, *env16;
  char *argarray[512], *envarray[512];
  char argblock[ARG_MAX], envblock[ENV_MAX];
  long auxarray[][2] = {{pushpop(0L), pushpop(0L)}};
  MitigateDriveByDownloads();
  NormalizeCmdExe();
  *(/*unconst*/ int *)&hostos = WINDOWS;
  cmd16 = GetCommandLine();
  env16 = GetEnvironmentStrings();
  count = GetDosArgv(cmd16, argblock, ARG_MAX, argarray, 512);
  for (i = 0; argarray[0][i]; ++i) {
    if (argarray[0][i] == '\\') argarray[0][i] = '/';
  }
  GetDosEnviron(env16, envblock, ENV_MAX, envarray, 512);
  FreeEnvironmentStrings(env16);
  _executive(count, argarray, envarray, auxarray);
}
