/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/fmt/libgen.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/getopt/getopt.internal.h"

static const struct Replacement {
  const char *from;
  const char *to;
} kReplacements[] = {
    // keep it sorted
    {"	addq	%fs:0, %r10", "	call	__add_tls_r10"},  //
    {"	addq	%fs:0, %r11", "	call	__add_tls_r11"},  //
    {"	addq	%fs:0, %r12", "	call	__add_tls_r12"},  //
    {"	addq	%fs:0, %r13", "	call	__add_tls_r13"},  //
    {"	addq	%fs:0, %r14", "	call	__add_tls_r14"},  //
    {"	addq	%fs:0, %r15", "	call	__add_tls_r15"},  //
    {"	addq	%fs:0, %r8", "	call	__add_tls_r8"},   //
    {"	addq	%fs:0, %r9", "	call	__add_tls_r9"},   //
    {"	addq	%fs:0, %rax", "	call	__add_tls_rax"},  //
    {"	addq	%fs:0, %rbp", "	call	__add_tls_rbp"},  //
    {"	addq	%fs:0, %rbx", "	call	__add_tls_rbx"},  //
    {"	addq	%fs:0, %rcx", "	call	__add_tls_rcx"},  //
    {"	addq	%fs:0, %rdi", "	call	__add_tls_rdi"},  //
    {"	addq	%fs:0, %rdx", "	call	__add_tls_rdx"},  //
    {"	addq	%fs:0, %rsi", "	call	__add_tls_rsi"},  //
    {"	movq	%fs:0, %r10", "	call	__get_tls_r10"},  //
    {"	movq	%fs:0, %r11", "	call	__get_tls_r11"},  //
    {"	movq	%fs:0, %r12", "	call	__get_tls_r12"},  //
    {"	movq	%fs:0, %r13", "	call	__get_tls_r13"},  //
    {"	movq	%fs:0, %r14", "	call	__get_tls_r14"},  //
    {"	movq	%fs:0, %r15", "	call	__get_tls_r15"},  //
    {"	movq	%fs:0, %r8", "	call	__get_tls_r8"},   //
    {"	movq	%fs:0, %r9", "	call	__get_tls_r9"},   //
    {"	movq	%fs:0, %rax", "	call	__get_tls_rax"},  //
    {"	movq	%fs:0, %rbp", "	call	__get_tls_rbp"},  //
    {"	movq	%fs:0, %rbx", "	call	__get_tls_rbx"},  //
    {"	movq	%fs:0, %rcx", "	call	__get_tls_rcx"},  //
    {"	movq	%fs:0, %rdi", "	call	__get_tls_rdi"},  //
    {"	movq	%fs:0, %rdx", "	call	__get_tls_rdx"},  //
    {"	movq	%fs:0, %rsi", "	call	__get_tls_rsi"},  //
};

struct Strings {
  size_t n;
  size_t c;
  const char **p;
};

static char *prog;
static int verbose;
static const char *input;

[[noreturn]] static void Die(const char *thing, const char *reason) {
  fprintf(stderr, "%s: fatal error: %s\n", thing, reason);
  exit(1);
}

[[noreturn]] static void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] static void DieOom(void) {
  Die(prog, "out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

static char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

static char *BaseName(const char *s) {
  return basename(StrDup(s));
}

static char *StripExt(const char *s) {
  char *p, *t;
  t = StrDup(s);
  if ((p = strrchr(t, '.')))
    *p = 0;
  return t;
}

static char *StrCat(const char *a, const char *b) {
  char *p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = Malloc(n + m + 1);
  if (n)
    memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

static void AppendString(struct Strings *l, const char *s) {
  if (l->n >= l->c) {
    l->c += 1;
    l->c += l->c >> 1;
    l->p = Realloc(l->p, l->c * sizeof(*l->p));
  }
  l->p[l->n++] = s;
}

static void PrintStrings(struct Strings *l) {
  for (size_t i = 0; i < l->n; ++i) {
    if (!l->p[i])
      break;
    if (i)
      fputc(' ', stderr);
    fputs(l->p[i], stderr);
  }
}

static bool IsJoinArg(const char *arg) {
  static const char *const kJoinArgs[] = {
      // keep it sorted
      "--param",             //
      "-A",                  //
      "-D",                  //
      "-I",                  //
      "-L",                  //
      "-MF",                 //
      "-MT",                 //
      "-T",                  //
      "-U",                  //
      "-Xpreprocessor",      //
      "-aux-info",           //
      "-dumpbase",           //
      "-dumpbase-ext",       //
      "-dumpdir",            //
      "-e",                  //
      "-idirafter",          //
      "-imacros",            //
      "-imultilib",          //
      "-include",            //
      "-iprefix",            //
      "-iquote",             //
      "-isysroot",           //
      "-isystem",            //
      "-iwithprefixbefore",  //
      "-l",                  //
      "-o",                  //
      "-wrapper",            //
      "-x",                  //
      "-z",                  //
  };
  int l = 0;
  int r = ARRAYLEN(kJoinArgs) - 1;
  while (l <= r) {
    int m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    int c = strcmp(kJoinArgs[m], arg);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return true;
    }
  }
  return false;
}

static const char *ReplaceLine(const char *line) {
  if (!strstr(line, "%fs"))
    return line;
  int l = 0;
  int r = ARRAYLEN(kReplacements) - 1;
  while (l <= r) {
    int m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    int c = strcmp(kReplacements[m].from, line);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return kReplacements[m].to;
    }
  }
  return line;
}

[[noreturn]] static void ShowUsage(FILE *f, int rc) {
  fprintf(f, "usage: %s CC ARGS...\n", prog);
  exit(rc);
}

int main(int argc, char *argv[]) {

  // get name of program
  if ((prog = argv[0])) {
    prog = basename(prog);
  } else {
    prog = "tlscc";
  }

  // get tlscc flags
  int opt;
  while ((opt = getopt(argc, argv, "vh")) != -1) {
    switch (opt) {
      case 'v':
        ++verbose;
        break;
      case 'h':
        ShowUsage(stdout, 0);
      default:
        ShowUsage(stderr, 1);
    }
  }
  if (optind == argc)
    Die(prog, "missing operand");

  uint64_t x;
  getentropy(&x, sizeof(x));
  char *asm_path = Malloc(PATH_MAX);
  snprintf(asm_path, PATH_MAX, "%stlscc-%lx.s", __get_tmpdir(), x);
  char *asm_path_fix = Malloc(PATH_MAX);
  snprintf(asm_path_fix, PATH_MAX, "%stlscc-%lx-fix.s", __get_tmpdir(), x);

  // copy and translate args
  // 1. check for -c arg to make sure it's compiling an object
  // 2. change -ofoo.o into -o/tmp/tlscc-xxxx.s
  // 3. change -o foo.o into -o/tmp/tlscc-xxxx.s
  bool is_compile = false;
  bool want_assembly = false;
  const char *is_not_compile = 0;
  int input_count = 0;
  const char *output = 0;
  const char *language = 0;
  struct Strings compile_args = {0};
  struct Strings assemble_args = {0};
  for (int i = optind; i < argc; ++i) {
    if (!is_compile)
      if (!strcmp(argv[i], "-c"))
        is_compile = true;
    if (!is_not_compile)
      if (!strcmp(argv[i], "-E") ||  //
          !strcmp(argv[i], "-M") ||  //
          !strcmp(argv[i], "-MM"))
        is_not_compile = argv[i];
    if (!want_assembly) {
      if (!strcmp(argv[i], "-S")) {
        want_assembly = true;
        is_compile = true;
      }
    }
    if (!strcmp(argv[i], "-o")) {
      if (i + 1 >= argc)
        Die(prog, "missing output argument");
      output = argv[++i];
    } else if (startswith(argv[i], "-o")) {
      output = argv[i] + 2;
    } else if (!strcmp(argv[i], "-x")) {
      if (i + 1 >= argc)
        Die(prog, "missing language argument");
      language = argv[++i];
    } else if (startswith(argv[i], "-x")) {
      language = argv[i] + 2;
    } else if (IsJoinArg(argv[i])) {
      if (i + 1 >= argc)
        Die(prog, "missing join argument");
      AppendString(&compile_args, argv[i]);
      AppendString(&assemble_args, argv[i]);
      ++i;
      AppendString(&compile_args, argv[i]);
      AppendString(&assemble_args, argv[i]);
    } else if (i == optind || argv[i][0] == '-') {
      AppendString(&compile_args, argv[i]);
      AppendString(&assemble_args, argv[i]);
    } else {
      input = argv[i];
      ++input_count;
    }
  }

  if (language && !strcmp(language, "none"))
    language = 0;

  if (verbose) {
    if (!is_compile) {
      fprintf(stderr, "%s: won't run tlscc because -c flag wasn't passed\n",
              input ? input : prog);
    } else if (is_not_compile) {
      fprintf(stderr,
              "%s: won't run tlscc because incompatible flag (%s) passed\n",
              input ? input : prog, is_not_compile);
    }
  }

  if (is_compile) {
    if (language) {
      if (!strcmp(language, "c") ||           //
          !strcmp(language, "c++") ||         //
          !strcmp(language, "cpp-output") ||  //
          !strcmp(language, "c++-cpp-output")) {
        // good
      } else {
        if (verbose)
          fprintf(stderr, "%s: unsupported language: %s\n",
                  input ? input : prog, language);
        is_compile = false;
      }
    } else if (input) {
      if (endswith(input, ".c") ||    //
          endswith(input, ".i") ||    //
          endswith(input, ".ii") ||   //
          endswith(input, ".cc") ||   //
          endswith(input, ".cp") ||   //
          endswith(input, ".cpp") ||  //
          endswith(input, ".CPP") ||  //
          endswith(input, ".cxx") ||  //
          endswith(input, ".c++") ||  //
          endswith(input, ".C")) {
        // good
      } else {
        if (verbose)
          fprintf(stderr, "%s: unsupported language file extension\n",
                  input ? input : prog);
        is_compile = false;
      }
    }
  }

  // if we're not compiling, then just delegate to compiler
  if (!is_compile || is_not_compile) {
    execvp(argv[optind], argv + optind);
    exit(127);
  }

  // validate usage
  if (!input)
    Die(prog, "missing input argument");
  if (input_count > 1)
    Die(prog, "too many input arguments");
  if (!output) {
    if (is_compile) {
      output = StrCat(StripExt(BaseName(input)), want_assembly ? ".s" : ".o");
    } else {
      output = "a.out";
    }
  }

  // make clang less annoying
  if (strstr(argv[optind], "clang")) {
    AppendString(&compile_args, "-Wno-unused-command-line-argument");
    AppendString(&assemble_args, "-Wno-unused-command-line-argument");
    // suppress all due to mysterious "inconsistent use of MD5 checksums"
    AppendString(&assemble_args, "-Wa,--no-warn");
  }

  // finish up arguments
  AppendString(&compile_args, "-mno-red-zone");
  AppendString(&compile_args, "-mno-tls-direct-seg-refs");
  AppendString(&compile_args, "-S");
  if (language)
    AppendString(&compile_args, StrCat("-x", language));
  AppendString(&compile_args, StrCat("-o", asm_path));
  AppendString(&assemble_args, StrCat("-o", output));
  AppendString(&compile_args, input);
  AppendString(&assemble_args, asm_path_fix);
  AppendString(&assemble_args, 0);
  AppendString(&compile_args, 0);

  // ctrl-c and ctrl-\ should kill child, not us
  sigset_t childmask;
  sigset_t parentmask;
  sigemptyset(&parentmask);
  sigaddset(&parentmask, SIGINT);
  sigaddset(&parentmask, SIGQUIT);
  sigaddset(&parentmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &parentmask, &childmask);

  if (verbose) {
    fprintf(stderr, "%s: tlscc spawning compile: ", input);
    PrintStrings(&compile_args);
    fputc('\n', stderr);
  }

  // spawn compiler
  switch (vfork()) {
    case 0:
      sigprocmask(SIG_SETMASK, &childmask, 0);
      execvp(compile_args.p[0], (char *const *)compile_args.p);
      _Exit(127);
    case -1:
      DieSys("vfork");
    default:
      break;
  }

  // wait for compiler
  int ws;
  if (wait(&ws) == -1)
    DieSys(prog);
  if (ws) {
    if (verbose)
      fprintf(stderr, "%s: tlscc compile wait status %#x\n", input, ws);
    if (WIFEXITED(ws))
      exit(WEXITSTATUS(ws));
    signal(WTERMSIG(ws), SIG_DFL);
    raise(WTERMSIG(ws));
    exit(128 + WTERMSIG(ws));
  }

  // rewrite tls instructions in assembly
  FILE *fin;
  if (!(fin = fopen(asm_path, "rb")))
    DieSys(asm_path);
  FILE *fout;
  if (!(fout = fopen(want_assembly ? output : asm_path_fix, "wb")))
    DieSys(want_assembly ? output : asm_path_fix);
  char *line;
  while ((line = chomp(fgetln(fin, 0)))) {
    fputs(ReplaceLine(line), fout);
    fputc('\n', fout);
  }
  fclose(fout);
  fclose(fin);

  if (!want_assembly) {
    if (verbose) {
      fprintf(stderr, "%s: tlscc spawning assembler: ", input);
      PrintStrings(&assemble_args);
      fputc('\n', stderr);
    }

    // spawn assembler
    switch (vfork()) {
      case 0:
        sigprocmask(SIG_SETMASK, &childmask, 0);
        execvp(assemble_args.p[0], (char *const *)assemble_args.p);
        _Exit(127);
      case -1:
        DieSys("vfork");
      default:
        break;
    }

    // wait for assembler
    if (wait(&ws) == -1)
      DieSys(prog);
    if (ws) {
      if (verbose)
        fprintf(stderr, "%s: tlscc assemble wait status %#x\n", input, ws);
      if (WIFEXITED(ws))
        exit(WEXITSTATUS(ws));
      signal(WTERMSIG(ws), SIG_DFL);
      raise(WTERMSIG(ws));
      exit(128 + WTERMSIG(ws));
    }
  }

  // cleanup
  if (!verbose) {
    if (!want_assembly)
      unlink(asm_path_fix);
    unlink(asm_path);
  }
}
