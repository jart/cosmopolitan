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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/calls/termios.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/nt/version.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__

textwindows int __munge_terminal_input(char *p, uint32_t *n) {
  size_t i, j;
  if (!(__ttymagic & kFdTtyNoCr2Nl)) {
    for (i = 0; i < *n; ++i) {
      if (p[i] == '\r') {
        p[i] = '\n';
      }
    }
  }
  bool delivered = false;
  bool got_vintr = false;
  bool got_vquit = false;
  for (j = i = 0; i < *n; ++i) {
    /*
       It's not possible to type Ctrl+Space (aka ^@) into the CMD.EXE
       console. Ctrl+Z is also problematic, since the Windows Console
       processes that keystroke into an EOF event, even when we don't
       enable input processing. These control codes are important for
       Emacs users. One solution is to install the "Windows Terminal"
       application from Microsoft Store, type Ctrl+Shift+, to get its
       settings.json file opened, and add this code to its "actions":

           "actions": [
               {
                   "command": {
                       "action": "sendInput",
                       "input": "␀"
                   },
                   "keys": "ctrl+space"
               },
               {
                   "command": {
                       "action": "sendInput",
                       "input": "␚"
                   },
                   "keys": "ctrl+z"
               },
               {
                   "command": {
                       "action": "sendInput",
                       "input": "\u001f"
                   },
                   "keys": "ctrl+-"
               }
           ],

       Its not possible to configure Windows Terminal to output our
       control codes. The workaround is to encode control sequences
       using the "Control Pictures" UNICODE plane, which we'll then
       translate back from UTF-8 glyphs, into actual control codes.

       Another option Windows users can consider, particularly when
       using CMD.EXE is installing Microsoft PowerTools whech makes
       it possible to remap keys then configure ncurses to use them

       https://github.com/microsoft/terminal/issues/2865
     */
    int c;
    if (i + 3 <= *n &&                // control pictures ascii nul ␀
        (p[i + 0] & 255) == 0xe2 &&   // becomes ^@ a.k.a. ctrl-space
        (p[i + 1] & 255) == 0x90 &&   // map the entire unicode plane
        ((p[i + 2] & 255) >= 0x80 &&  //
         (p[i + 2] & 255) <= 0x9F)) {
      c = (p[i + 2] & 255) - 0x80;
      i += 2;
    } else {
      c = p[i] & 255;
    }
    if (!(__ttymagic & kFdTtyNoIsigs) &&  //
        __vintr != _POSIX_VDISABLE &&     //
        c == __vintr) {
      got_vintr = true;
    } else if (!(__ttymagic & kFdTtyNoIsigs) &&  //
               __vquit != _POSIX_VDISABLE &&     //
               c == __vquit) {
      got_vquit = true;
    } else {
      p[j++] = c;
    }
  }
  if (got_vintr) {
    delivered |= __sig_handle(0, SIGINT, SI_KERNEL, 0);
  }
  if (got_vquit) {
    delivered |= __sig_handle(0, SIGQUIT, SI_KERNEL, 0);
  }
  if (*n && !j) {
    if (delivered) {
      return DO_EINTR;
    } else {
      return DO_RESTART;
    }
  }
  *n = j;
  return DO_NOTHING;
}

// Manual CMD.EXE echoing for when !ICANON && ECHO is the case.
textwindows void __echo_terminal_input(struct Fd *fd, char *p, size_t n) {
  int64_t hOutput;
  if (fd->kind == kFdConsole) {
    hOutput = fd->extra;
  } else {
    hOutput = g_fds.p[1].handle;
  }
  if (__ttymagic & kFdTtyEchoRaw) {
    WriteFile(hOutput, p, n, 0, 0);
  } else {
    size_t i;
    for (i = 0; i < n; ++i) {
      if (isascii(p[i]) && iscntrl(p[i]) && p[i] != '\n' && p[i] != '\t') {
        char ctl[2];
        ctl[0] = '^';
        ctl[1] = p[i] ^ 0100;
        WriteFile(hOutput, ctl, 2, 0, 0);
      } else {
        WriteFile(hOutput, p + i, 1, 0, 0);
      }
    }
  }
}

textwindows int tcsetattr_nt(int fd, int opt, const struct termios *tio) {
  bool32 ok;
  int64_t hInput, hOutput;
  uint32_t inmode, outmode;

  if (__isfdkind(fd, kFdConsole)) {
    hInput = g_fds.p[fd].handle;
    hOutput = g_fds.p[fd].extra;
  } else if (fd == STDIN_FILENO ||   //
             fd == STDOUT_FILENO ||  //
             fd == STDERR_FILENO) {
    hInput = g_fds.p[STDIN_FILENO].handle;
    hOutput = g_fds.p[MAX(STDOUT_FILENO, fd)].handle;
  } else {
    return enotty();
  }

  if (!GetConsoleMode(hInput, &inmode) || !GetConsoleMode(hOutput, &outmode)) {
    return enotty();
  }

  if (opt == TCSAFLUSH) {
    tcflush(fd, TCIFLUSH);
  }
  inmode &= ~(kNtEnableLineInput | kNtEnableEchoInput |
              kNtEnableProcessedInput | kNtEnableVirtualTerminalInput);
  inmode |= kNtEnableWindowInput;
  __ttymagic = 0;
  if (tio->c_lflag & ICANON) {
    inmode |= kNtEnableLineInput | kNtEnableProcessedInput;
  } else {
    __ttymagic |= kFdTtyMunging;
    if (tio->c_cc[VMIN] != 1) {
      STRACE("tcsetattr c_cc[VMIN] must be 1 on Windows");
      return einval();
    }
    if (IsAtLeastWindows10()) {
      // - keys like f1, up, etc. get turned into \e ansi codes
      // - totally destroys default console gui (e.g. up arrow)
      inmode |= kNtEnableVirtualTerminalInput;
    }
  }
  if (!(tio->c_iflag & ICRNL)) {
    __ttymagic |= kFdTtyNoCr2Nl;
  }
  if (!(tio->c_lflag & ECHOCTL)) {
    __ttymagic |= kFdTtyEchoRaw;
  }
  if (tio->c_lflag & ECHO) {
    // "kNtEnableEchoInput can be used only if the
    //  kNtEnableLineInput mode is also enabled." -MSDN
    if (tio->c_lflag & ICANON) {
      inmode |= kNtEnableEchoInput;
    } else {
      // If ECHO is enabled in raw mode, then read(0) needs to
      // magically write(1) to simulate echoing. This normally
      // visualizes control codes, e.g. \r → ^M unless ECHOCTL
      // hasn't been specified.
      __ttymagic |= kFdTtyEchoing;
    }
  }
  if (!(tio->c_lflag & ISIG)) {
    __ttymagic |= kFdTtyNoIsigs;
  }
  __vintr = tio->c_cc[VINTR];
  __vquit = tio->c_cc[VQUIT];
  if ((tio->c_lflag & ISIG) &&  //
      tio->c_cc[VINTR] == CTRL('C')) {
    // allows ctrl-c to be delivered asynchronously via win32
    inmode |= kNtEnableProcessedInput;
  }
  ok = SetConsoleMode(hInput, inmode);
  (void)ok;
  NTTRACE("SetConsoleMode(%p, %s) → %hhhd", hInput,
          DescribeNtConsoleInFlags(inmode), ok);

  outmode &= ~kNtDisableNewlineAutoReturn;
  outmode |= kNtEnableProcessedOutput;
  if (!(tio->c_oflag & ONLCR)) {
    outmode |= kNtDisableNewlineAutoReturn;
  }
  if (IsAtLeastWindows10()) {
    outmode |= kNtEnableVirtualTerminalProcessing;
  }
  ok = SetConsoleMode(hOutput, outmode);
  (void)ok;
  NTTRACE("SetConsoleMode(%p, %s) → %hhhd", hOutput,
          DescribeNtConsoleOutFlags(outmode), ok);

  return 0;
}

__attribute__((__constructor__)) static void tcsetattr_nt_init(void) {
  if (!getenv("TERM")) {
    setenv("TERM", "xterm-256color", true);
  }
}

#endif /* __x86_64__ */
