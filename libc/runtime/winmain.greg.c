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
#include "libc/runtime/directmap.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

/*
 * TODO: Why can't we allocate addresses above 4GB on Windows 7 x64?
 * TODO: How can we ensure we never overlap with KERNEL32.DLL?
 */

#define WINSTACK 0x100000

struct WinArgs {
  char *argv[4096];
  char *envp[4096];
  char argblock[ARG_MAX];
  char envblock[ARG_MAX];
};

static textwindows void SetTrueColor(void) {
  SetEnvironmentVariable(u"TERM", u"xterm-truecolor");
}

static textwindows void MakeLongDoubleLongAgain(void) {
  int x87cw = 0x037f; /* let's hope win32 won't undo this */
  asm volatile("fldcw\t%0" : /* no outputs */ : "m"(x87cw));
}

static textwindows void NormalizeCmdExe(void) {
  uint32_t mode;
  int64_t handle, hstdin, hstdout, hstderr;
  if ((int)weakaddr("v_ntsubsystem") == kNtImageSubsystemWindowsCui &&
      NtGetVersion() >= kNtVersionWindows10) {
    hstdin = GetStdHandle(pushpop(kNtStdInputHandle));
    hstdout = GetStdHandle(pushpop(kNtStdOutputHandle));
    hstderr = GetStdHandle(pushpop(kNtStdErrorHandle));
    if (GetFileType((handle = hstdin)) == kNtFileTypeChar) {
      SetTrueColor();
      SetConsoleCP(kNtCpUtf8);
      GetConsoleMode(handle, &mode);
      SetConsoleMode(handle, mode | kNtEnableProcessedInput |
                                 kNtEnableEchoInput | kNtEnableLineInput |
                                 kNtEnableWindowInput |
                                 kNtEnableVirtualTerminalInput);
    }
    if (GetFileType((handle = hstdout)) == kNtFileTypeChar ||
        GetFileType((handle = hstderr)) == kNtFileTypeChar) {
      SetTrueColor();
      SetConsoleOutputCP(kNtCpUtf8);
      GetConsoleMode(handle, &mode);
      SetConsoleMode(handle, mode | kNtEnableProcessedOutput |
                                 kNtEnableWrapAtEolOutput |
                                 (NtGetVersion() >= kNtVersionWindows10
                                      ? kNtEnableVirtualTerminalProcessing
                                      : 0));
    }
  }
}

static textwindows wontreturn void WinMainNew(void) {
  int64_t h;
  size_t size;
  int i, count;
  uint64_t addr;
  long auxv[1][2];
  struct WinArgs *wa;
  const char16_t *env16;
  NormalizeCmdExe();
  *(/*unconst*/ int *)&__hostos = WINDOWS;
  addr = NtGetVersion() < kNtVersionWindows10 ? 0xff00000 : 0x777000000000;
  size = ROUNDUP(WINSTACK + sizeof(struct WinArgs), FRAMESIZE);
  _mmi.p[0].h =
      __mmap$nt((char *)addr, size, PROT_READ | PROT_WRITE | PROT_EXEC, -1, 0)
          .maphandle;
  _mmi.p[0].x = addr >> 16;
  _mmi.p[0].y = (addr >> 16) + ((size >> 16) - 1);
  _mmi.p[0].prot = PROT_READ | PROT_WRITE | PROT_EXEC;
  _mmi.p[0].flags = MAP_PRIVATE | MAP_ANONYMOUS;
  _mmi.i = pushpop(1L);
  wa = (struct WinArgs *)(addr + size - sizeof(struct WinArgs));
  count = GetDosArgv(GetCommandLine(), wa->argblock, ARG_MAX, wa->argv, 4096);
  for (i = 0; wa->argv[0][i]; ++i) {
    if (wa->argv[0][i] == '\\') {
      wa->argv[0][i] = '/';
    }
  }
  env16 = GetEnvironmentStrings();
  GetDosEnviron(env16, wa->envblock, ARG_MAX, wa->envp, 4096);
  FreeEnvironmentStrings(env16);
  auxv[0][0] = pushpop(0L);
  auxv[0][1] = pushpop(0L);
  _jmpstack((char *)addr + WINSTACK, _executive, count, wa->argv, wa->envp,
            auxv);
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
 * 6. Reconfigure x87 FPU so long double is actually long (80 bits).
 *
 * 7. Finally, we need fork. Microsoft designed Windows to prevent us
 *    from having fork() so we pass pipe handles in an environment
 *    variable literally copy all the memory.
 *
 * @param hInstance call GetModuleHandle(NULL) from main if you need it
 */
textwindows int64_t WinMain(int64_t hInstance, int64_t hPrevInstance,
                            const char *lpCmdLine, int nCmdShow) {
  MakeLongDoubleLongAgain();
  if (weaken(winsockinit)) weaken(winsockinit)();
  if (weaken(WinMainForked)) weaken(WinMainForked)();
  WinMainNew();
}
