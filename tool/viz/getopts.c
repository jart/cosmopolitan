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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview Tool for printing which getopt() letters are used, e.g.
 *
 *     o//tool/viz/getopts.com jkazhdugvVsmbBfe:A:l:p:r:R:H
 */

int letters_used[256];
bool letters_with_args[256];

bool IsLegal(int c) {
  return isgraph(c) && c != ':' && c != '?';
}

int main(int argc, char *argv[]) {
  int i, j;
  bool hasargless;

  for (i = 1; i < argc; ++i) {
    for (j = 0; argv[i][j]; ++j) {
      ++letters_used[argv[i][j] & 255];
      if (argv[i][j + 1] == ':') {
        letters_with_args[argv[i][j] & 255] = true;
      }
    }
  }

  // usage report
  fprintf(stderr, "// options used: ");
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i]) {
      if (j++) fprintf(stderr, "");
      fprintf(stderr, "%c", i);
    }
  }
  if (!j) fprintf(stderr, "none");
  fprintf(stderr, "\n");
  fprintf(stderr, "// letters not used: ");
  for (j = i = 0; i < 128; ++i) {
    if (!isalpha(i)) continue;
    if (!letters_used[i]) {
      if (j++) fprintf(stderr, "");
      fprintf(stderr, "%c", i);
    }
  }
  if (!j) fprintf(stderr, "none");
  fprintf(stderr, "\n");
  fprintf(stderr, "// digits not used: ");
  for (j = i = 0; i < 128; ++i) {
    if (!isdigit(i)) continue;
    if (!letters_used[i]) {
      if (j++) fprintf(stderr, "");
      fprintf(stderr, "%c", i);
    }
  }
  if (!j) fprintf(stderr, "none");
  fprintf(stderr, "\n");
  fprintf(stderr, "// puncts not used: ");
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (isalnum(i)) continue;
    if (!letters_used[i]) {
      if (j++) fprintf(stderr, "");
      fprintf(stderr, "%c", i);
    }
  }
  if (!j) fprintf(stderr, "none");
  fprintf(stderr, "\n");
  fprintf(stderr, "// letters duplicated: ");
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i] > 1) {
      if (j++) fprintf(stderr, "");
      fprintf(stderr, "%c", i);
    }
  }
  if (!j) fprintf(stderr, "none");
  fprintf(stderr, "\n");

  // generated code
  hasargless = false;
  printf("#define GETOPTS \"");
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i] && !letters_with_args[i]) {
      printf("%c", i);
      hasargless = true;
    }
  }
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i] && letters_with_args[i]) {
      printf("%c:", i);
    }
  }
  printf("\"\n");
  printf("\n#define USAGE \"\\\n");
  printf("Usage: program.com");
  if (hasargless) {
    printf(" [-");
    for (j = i = 0; i < 128; ++i) {
      if (!IsLegal(i)) continue;
      if (letters_used[i] && !letters_with_args[i]) {
        printf("%c", i);
        hasargless = true;
      }
    }
    printf("]");
  }
  printf(" ARGS...\\n\\\n");
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i] && !letters_with_args[i]) {
      printf("  -%c          the %c option\\n\\\n", i, i);
    }
  }
  for (j = i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_with_args[i]) {
      printf("  -%c VAL      the %c option\\n\\\n", i, i);
    }
  }
  printf("\"\n\n");

  for (i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i]) {
      if (letters_with_args[i]) {
        printf("const char *");
      } else {
        printf("int ");
      }
      if (isalnum(i) || i == '_') {
        printf("g_%cflag;\n", i);
      }
    }
  }

  printf("\n\
static void GetOpts(int argc, char *argv[]) {\n\
  int opt;\n\
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {\n\
    switch (opt) {\n");
  for (i = 0; i < 128; ++i) {
    if (!IsLegal(i)) continue;
    if (letters_used[i]) {
      printf("      case '%c':\n", i);
      if (isalnum(i) || i == '_') {
        printf("        g_%cflag", i);
      } else {
        printf("        g_XXXflag", i);
      }
      if (letters_with_args[i]) {
        printf(" = optarg;\n");
      } else {
        printf("++;\n");
      }
      printf("        break;\n");
    }
  }

  printf("      case '?':\n");
  printf("        write(1, USAGE, sizeof(USAGE) - 1);\n");
  printf("        exit(0);\n");
  printf("      default:\n");
  printf("        write(2, USAGE, sizeof(USAGE) - 1);\n");
  printf("        exit(64);\n");
  printf("    }\n");
  printf("  }\n");
  printf("}\n");
  printf("\n");

  printf("int main(int argc, char *argv[]) {\n");
  printf("  GetOpts(argc, argv);\n");
  printf("}\n");

  return 0;
}
