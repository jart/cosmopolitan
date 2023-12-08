/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/termios.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/termios.h"

#define N 1024

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeTermios)(char buf[N], ssize_t rc,
                              const struct termios *tio) {
  int o = 0;
  char b128[128];

  if (!tio) return "NULL";
  if ((!IsAsan() && kisdangerous(tio)) ||
      (IsAsan() && !__asan_is_valid(tio, sizeof(*tio)))) {
    ksnprintf(buf, N, "%p", tio);
    return buf;
  }

  append("{");

  struct DescribeFlags kInput[] = {
      {IGNBRK, "IGNBRK"},    //
      {BRKINT, "BRKINT"},    //
      {IGNPAR, "IGNPAR"},    //
      {PARMRK, "PARMRK"},    //
      {INPCK, "INPCK"},      //
      {ISTRIP, "ISTRIP"},    //
      {INLCR, "INLCR"},      //
      {IGNCR, "IGNCR"},      //
      {ICRNL, "ICRNL"},      //
      {IUCLC, "IUCLC"},      //
      {IXON, "IXON"},        //
      {IXANY, "IXANY"},      //
      {IXOFF, "IXOFF"},      //
      {IMAXBEL, "IMAXBEL"},  //
      {IUTF8, "IUTF8"},      //
  };
  append(".c_iflag=%s",
         DescribeFlags(b128, 128, kInput, ARRAYLEN(kInput), "", tio->c_iflag));

  struct DescribeFlags kOutput[] = {
      {OPOST, "OPOST"},    //
      {OLCUC, "OLCUC"},    //
      {ONLCR, "ONLCR"},    //
      {OCRNL, "OCRNL"},    //
      {ONOCR, "ONOCR"},    //
      {ONLRET, "ONLRET"},  //
      {OFILL, "OFILL"},    //
      {OFDEL, "OFDEL"},    //
      {NL1, "NL1"},        //
      {CR3, "CR3"},        //
      {CR2, "CR2"},        //
      {CR1, "CR1"},        //
      {TAB3, "TAB3"},      //
      {TAB2, "TAB2"},      //
      {TAB1, "TAB1"},      //
      {BS1, "BS1"},        //
      {VT1, "VT1"},        //
      {FF1, "FF1"},        //
  };
  append(", .c_oflag=%s", DescribeFlags(b128, 128, kOutput, ARRAYLEN(kOutput),
                                        "", tio->c_oflag));

  struct DescribeFlags kControl[] = {
      {CS8, "CS8"},          //
      {CS7, "CS7"},          //
      {CS6, "CS6"},          //
      {CSTOPB, "CSTOPB"},    //
      {CREAD, "CREAD"},      //
      {PARENB, "PARENB"},    //
      {PARODD, "PARODD"},    //
      {HUPCL, "HUPCL"},      //
      {CLOCAL, "CLOCAL"},    //
      {CRTSCTS, "CRTSCTS"},  //
  };
  append(", .c_cflag=%s", DescribeFlags(b128, 128, kControl, ARRAYLEN(kControl),
                                        "", tio->c_cflag));

  struct DescribeFlags kLocal[] = {
      {ISIG, "ISIG"},        //
      {ICANON, "ICANON"},    //
      {XCASE, "XCASE"},      //
      {ECHO, "ECHO"},        //
      {ECHOE, "ECHOE"},      //
      {ECHOK, "ECHOK"},      //
      {ECHONL, "ECHONL"},    //
      {NOFLSH, "NOFLSH"},    //
      {TOSTOP, "TOSTOP"},    //
      {ECHOCTL, "ECHOCTL"},  //
      {ECHOPRT, "ECHOPRT"},  //
      {ECHOKE, "ECHOKE"},    //
      {FLUSHO, "FLUSHO"},    //
      {PENDIN, "PENDIN"},    //
      {IEXTEN, "IEXTEN"},    //
      {EXTPROC, "EXTPROC"},  //
  };
  append(", "
         ".c_lflag=%s, "
         ".c_cc[VMIN]=%d, "
         ".c_cc[VTIME]=%d, "
         ".c_cc[VINTR]=CTRL(%#c), "
         ".c_cc[VQUIT]=CTRL(%#c)",
         DescribeFlags(b128, 128, kLocal, ARRAYLEN(kLocal), "", tio->c_lflag),
         tio->c_cc[VMIN], tio->c_cc[VTIME], CTRL(tio->c_cc[VINTR]),
         CTRL(tio->c_cc[VQUIT]));

  append("}");

  return buf;
}
