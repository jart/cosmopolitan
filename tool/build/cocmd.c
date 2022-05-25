/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

/**
 * @fileoverview Cosmopolitan Command Interpreter
 *
 * This is a lightweight command interpreter for GNU Make. It has just
 * enough shell script language support to support our build config.
 */

#define STATE_SHELL 0
#define STATE_STR   1

char *p;
char *q;
char *cmd;
char *args[8192];
const char *prog;
char argbuf[ARG_MAX];
bool unsupported[256];

wontreturn void UnsupportedSyntax(unsigned char c) {
  char ibuf[13];
  FormatOctal32(ibuf, c, true);
  fputs(prog, stderr);
  fputs(": unsupported shell syntax '", stderr);
  fputc(c, stderr);
  fputs("' (", stderr);
  fputs(ibuf, stderr);
  fputs("): ", stderr);
  fputs(cmd, stderr);
  fputs("\n", stderr);
  exit(1);
}

void Open(const char *path, int fd, int flags) {
  const char *err;
  close(fd);
  if (open(path, flags, 0644) == -1) {
    err = strerdoc(errno);
    fputs(prog, stderr);
    fputs(": failed to open '", stderr);
    fputs(path, stderr);
    fputs("': ", stderr);
    fputs(err, stderr);
    fputs("\n", stderr);
    exit(1);
  }
}

char *Tokenize(void) {
  int t;
  char *r;
  while (*p == ' ' || *p == '\t' || *p == '\n' ||
         (p[0] == '\\' && p[1] == '\n')) {
    ++p;
  }
  if (!*p) return 0;
  t = STATE_SHELL;
  for (r = q;; ++p) {
    switch (t) {

      case STATE_SHELL:
        if (unsupported[*p & 255]) {
          UnsupportedSyntax(*p);
        }
        if (!*p || *p == ' ' || *p == '\t') {
          *q++ = 0;
          return r;
        } else if (*p == '\'') {
          t = STATE_STR;
        } else if (*p == '\\') {
          if (!p[1]) UnsupportedSyntax(*p);
          *q++ = *++p;
        } else {
          *q++ = *p;
        }
        break;

      case STATE_STR:
        if (!*p) {
          fputs("cmd: error: unterminated string\n", stderr);
          exit(1);
        }
        if (*p == '\'') {
          t = STATE_SHELL;
        } else {
          *q++ = *p;
        }
        break;

      default:
        unreachable;
    }
  }
}

int main(int argc, char *argv[]) {
  char *s, *arg;
  size_t i, j, n;
  prog = argc > 0 ? argv[0] : "cocmd.com";

  for (i = 1; i < 32; ++i) {
    unsupported[i] = true;
  }
  unsupported['\t'] = false;
  unsupported[0177] = true;
  unsupported['~'] = true;
  unsupported['`'] = true;
  unsupported['#'] = true;
  unsupported['$'] = true;
  unsupported['*'] = true;
  unsupported['('] = true;
  unsupported[')'] = true;
  unsupported['|'] = true;
  unsupported['['] = true;
  unsupported[']'] = true;
  unsupported['{'] = true;
  unsupported['}'] = true;
  unsupported[';'] = true;
  unsupported['"'] = true;
  unsupported['?'] = true;
  unsupported['!'] = true;

  if (argc != 3) {
    fputs(prog, stderr);
    fputs(": error: wrong number of args\n", stderr);
    return 1;
  }

  if (strcmp(argv[1], "-c")) {
    fputs(prog, stderr);
    fputs(": error: argv[1] should -c\n", stderr);
    return 1;
  }

  p = cmd = argv[2];
  if (strlen(cmd) >= ARG_MAX) {
    fputs(prog, stderr);
    fputs(": error: cmd too long: ", stderr);
    fputs(cmd, stderr);
    fputs("\n", stderr);
    return 1;
  }

  n = 0;
  q = argbuf;
  while ((arg = Tokenize())) {
    if (n + 1 < ARRAYLEN(args)) {
      if (!strcmp(arg, "2>&1")) {
        close(1);
        dup(2);
      } else if (!strcmp(arg, ">&2")) {
        close(2);
        dup(1);
      } else if (arg[0] == '2' && arg[1] == '>' && arg[2] == '>') {
        Open(arg + 3, 2, O_WRONLY | O_CREAT | O_APPEND);
      } else if (arg[0] == '>' && arg[1] == '>') {
        Open(arg + 2, 1, O_WRONLY | O_CREAT | O_APPEND);
      } else if (arg[0] == '2' && arg[1] == '>') {
        Open(arg + 2, 2, O_WRONLY | O_CREAT | O_TRUNC);
      } else if (arg[0] == '>') {
        Open(arg + 1, 1, O_WRONLY | O_CREAT | O_TRUNC);
      } else if (arg[0] == '<') {
        Open(arg + 1, 0, O_RDONLY);
      } else {
        args[n++] = arg;
      }
    } else {
      fputs(prog, stderr);
      fputs(": error: too many args\n", stderr);
      return 1;
    }
  }

  if (!n) {
    fputs(prog, stderr);
    fputs(": error: too few args\n", stderr);
    return 1;
  }

  execv(args[0], args);
  if (!n) {
    s = strerdoc(errno);
    fputs(prog, stderr);
    fputs(": execve '", stderr);
    fputs(args[0], stderr);
    fputs("' failed: ", stderr);
    fputs(s, stderr);
    fputs("\n", stderr);
    return 1;
  }

  return 127;
}
