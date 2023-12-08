/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/runtime.h"
#if defined(__x86_64__) && SupportsWindows()

int main(int argc, char *argv[]) {

  if (!IsWindows()) {
    tinyprint(2, argv[0], " is intended for windows\n", NULL);
    return 1;
  }

  // clear console and print old config
  uint32_t cm;
  tinyprint(1, "\e[H\e[J", NULL);
  tinyprint(1, "broken console settings were\r\n", NULL);
  GetConsoleMode(GetStdHandle(kNtStdInputHandle), &cm);
  tinyprint(1, "stdin: ", DescribeNtConsoleInFlags(cm), "\r\n", NULL);
  GetConsoleMode(GetStdHandle(kNtStdOutputHandle), &cm);
  tinyprint(1, "stdout: ", DescribeNtConsoleOutFlags(cm), "\r\n", NULL);
  GetConsoleMode(GetStdHandle(kNtStdErrorHandle), &cm);
  tinyprint(1, "stderr: ", DescribeNtConsoleOutFlags(cm), "\r\n", NULL);

  // fix console settings
  SetConsoleMode(GetStdHandle(kNtStdInputHandle),
                 kNtEnableProcessedInput | kNtEnableLineInput |
                     kNtEnableEchoInput | kNtEnableMouseInput |
                     kNtEnableInsertMode | kNtEnableQuickEditMode |
                     kNtEnableExtendedFlags | kNtEnableAutoPosition);
  SetConsoleMode(GetStdHandle(kNtStdOutputHandle),
                 kNtEnableProcessedOutput | kNtEnableWrapAtEolOutput);
  SetConsoleMode(GetStdHandle(kNtStdErrorHandle),
                 kNtEnableProcessedOutput | kNtEnableWrapAtEolOutput |
                     kNtEnableVirtualTerminalProcessing);

  return 0;
}

#else
int main(int argc, char *argv[]) {
  tinyprint(2, "fixconsole not supported on this cpu arch or build config\n",
            NULL);
  return 1;
}
#endif /* __x86_64__ && SupportsWindows() */
