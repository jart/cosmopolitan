/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/bits/pushpop.h"
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/loadlibrarysearch.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/pedef.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

struct WinArgs {
  char *argv[512];
  char *envp[512];
  long auxv[1][2];
  char argblock[ARG_MAX];
  char envblock[ENV_MAX];
};

static struct CmdExe {
  bool result;
  struct OldNtConsole {
    uint32_t codepage;
    uint32_t mode;
    int64_t handle;
  } oldin, oldout;
} g_cmdexe;

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

static textwindows char *AllocateMemory(void *addr, size_t size, int64_t *h) {
  return MapViewOfFileExNuma(
      (*h = CreateFileMappingNuma(-1, NULL, pushpop(kNtPageExecuteReadwrite), 0,
                                  size, NULL, kNtNumaNoPreferredNode)),
      kNtFileMapRead | kNtFileMapWrite, 0, 0, size, addr,
      kNtNumaNoPreferredNode);
}

static textwindows wontreturn void WinMainNew(void) {
  int64_t h;
  size_t size;
  int i, count;
  uint64_t data;
  struct WinArgs *wa;
  const char16_t *env16;
  NormalizeCmdExe();
  *(/*unconst*/ int *)&__hostos = WINDOWS;
  size = ROUNDUP(STACKSIZE + sizeof(struct WinArgs), FRAMESIZE);
  data = (intptr_t)AllocateMemory((char *)0x777000000000, size, &_mmi.p[0].h);
  _mmi.p[0].x = data >> 16;
  _mmi.p[0].y = (data >> 16) + ((size >> 16) - 1);
  _mmi.p[0].prot = PROT_READ | PROT_WRITE;
  _mmi.p[0].flags = MAP_PRIVATE | MAP_ANONYMOUS;
  _mmi.i = pushpop(1L);
  wa = (struct WinArgs *)(data + size - sizeof(struct WinArgs));
  count = GetDosArgv(GetCommandLine(), wa->argblock, ARG_MAX, wa->argv, 512);
  for (i = 0; wa->argv[0][i]; ++i) {
    if (wa->argv[0][i] == '\\') {
      wa->argv[0][i] = '/';
    }
  }
  env16 = GetEnvironmentStrings();
  GetDosEnviron(env16, wa->envblock, ENV_MAX, wa->envp, 512);
  FreeEnvironmentStrings(env16);
  wa->auxv[0][0] = pushpop(0L);
  wa->auxv[0][1] = pushpop(0L);
  _jmpstack((char *)data + STACKSIZE, _executive, count, wa->argv, wa->envp,
            wa->auxv);
}

/**
 * Main function on Windows NT.
 *
 * The Cosmopolitan Runtime provides the following services, which aim
 * to bring Windows NT behavior closer in harmony with System Five:
 *
 * 1. We configure CMD.EXE for UTF-8 and enable ANSI colors on Win10.
 *
 * 2. Command line arguments are passed as a blob of UTF-16 text. We
 *    chop them up into an char *argv[] UTF-8 data structure, in
 *    accordance with the DOS conventions for argument quoting.
 *
 * 3. Environment variables are passed to us as a sorted UTF-16 double
 *    NUL terminated list. We translate this to char ** using UTF-8.
 *
 * 4. Allocates new stack at a high address. NT likes to choose a
 *    stack address that's beneath the program image. We want to be
 *    able to assume that stack addresses are located at higher
 *    addresses than heap and program memory.
 *
 * 5. Windows users are afraid of "drive-by downloads" where someone
 *    might accidentally an evil DLL to their Downloads folder which
 *    then overrides the behavior of a legitimate EXE being run from
 *    the downloads folder. Since we don't even use dynamic linking,
 *    we've cargo culted some API calls, that may harden against it.
 *
 * 6. Finally, we need fork. Microsoft designed Windows to prevent us
 *    from having fork() so we pass pipe handles in an environment
 *    variable literally copy all the memory.
 *
 * @param hInstance call GetModuleHandle(NULL) from main if you need it
 */
textwindows int64_t WinMain(int64_t hInstance, int64_t hPrevInstance,
                            const char *lpCmdLine, int nCmdShow) {
  SetDefaultDllDirectories(kNtLoadLibrarySearchSearchSystem32);
  if (weaken(winsockinit)) weaken(winsockinit)();
  if (weaken(WinMainForked)) weaken(WinMainForked)();
  WinMainNew();
}
