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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/editline/editline.h"
#include "third_party/editline/internal.h"

asm(".ident\t\"\\n\\n\
Cosmopolitan Linenoise (BSD-like license)\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Copyright 1992,1993 Simmule Turner and Rich Salz\\n\
All rights reserved.\\n\
\\n\
This software is not subject to any license of the American Telephone\\n\
and Telegraph Company or of the Regents of the University of California.\\n\
\\n\
Permission is granted to anyone to use this software for any purpose on\\n\
any computer system, and to alter it and redistribute it freely, subject\\n\
to the following restrictions:\\n\
1. The authors are not responsible for the consequences of use of this\\n\
   software, no matter how awful, even if they arise from flaws in it.\\n\
2. The origin of this software must not be misrepresented, either by\\n\
   explicit claim or by omission.  Since few users ever read sources,\\n\
   credits must appear in the documentation.\\n\
3. Altered versions must be plainly marked as such, and must not be\\n\
   misrepresented as being the original software.  Since few users\\n\
   ever read sources, credits must appear in the documentation.\\n\
4. This notice may not be removed or altered.\"");
asm(".include \"libc/disclaimer.inc\"");

/* Wrapper for tcgetattr */
static int getattr(int fd, struct termios *arg) {
  int result, retries = 3;
  while (-1 == (result = tcgetattr(fd, arg)) && retries > 0) {
    retries--;
    if (EINTR == errno) continue;
    break;
  }
  return result;
}

/* Wrapper for tcgetattr */
static int setattr(int fd, int opt, const struct termios *arg) {
  int result, retries = 3;
  while (-1 == (result = tcsetattr(fd, opt, arg)) && retries > 0) {
    retries--;
    if (EINTR == errno) continue;
    break;
  }
  return result;
}

void rl_ttyset(int Reset) {
  static struct termios old;
  struct termios new;
  if (!Reset) {
    if (-1 == getattr(0, &old)) {
      perror("Failed tcgetattr()");
    }
    rl_erase = old.c_cc[VERASE];
    rl_kill = old.c_cc[VKILL];
    rl_eof = old.c_cc[VEOF];
    rl_intr = old.c_cc[VINTR];
    rl_quit = old.c_cc[VQUIT];
#ifdef CONFIG_SIGSTOP
    rl_susp = old.c_cc[VSUSP];
#endif
    new = old;
    new.c_lflag &= ~(ECHO | ICANON | ISIG);
    new.c_iflag &= ~INPCK;
    if (rl_meta_chars)
      new.c_iflag |= ISTRIP;
    else
      new.c_iflag &= ~ISTRIP;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    if (-1 == setattr(0, TCSADRAIN, &new)) {
      perror("Failed tcsetattr(TCSADRAIN)");
    }
  } else {
    if (-1 == setattr(0, TCSADRAIN, &old)) {
      perror("Failed tcsetattr(TCSADRAIN)");
    }
  }
}

void rl_add_slash(char *path, char *p) {
  struct stat Sb;
  if (stat(path, &Sb) >= 0) strcat(p, S_ISDIR(Sb.st_mode) ? "/" : " ");
}
