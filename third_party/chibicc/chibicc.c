#include "third_party/chibicc/chibicc.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/fmt/libgen.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/xasprintf.h"

asm(".ident\t\"\\n\\n\
chibicc (MIT/ISC License)\\n\
Copyright 2019 Rui Ueyama\\n\
Copyright 2020 Justine Alexandra Roberts Tunney\"");
asm(".include \"libc/disclaimer.inc\"");

typedef enum {
  FILE_NONE,
  FILE_C,
  FILE_ASM,
  FILE_ASM_CPP,
  FILE_OBJ,
  FILE_AR,
  FILE_DSO,
} FileType;

bool opt_common = true;
bool opt_data_sections;
bool opt_fentry;
bool opt_function_sections;
bool opt_no_builtin;
bool opt_nop_mcount;
bool opt_pg;
bool opt_pic;
bool opt_popcnt;
bool opt_record_mcount;
bool opt_sse3;
bool opt_sse4;
bool opt_verbose;

static bool opt_A;
static bool opt_E;
static bool opt_J;
static bool opt_P;
static bool opt_M;
static bool opt_MD;
static bool opt_MMD;
static bool opt_MP;
static bool opt_S;
static bool opt_c;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static bool opt_static;
static bool opt_save_temps;
static char *opt_MF;
static char *opt_MT;
static char *opt_o;
static FileType opt_x;
static StringArray opt_include;

StringArray include_paths;
static StringArray ld_extra_args;
static StringArray as_extra_args;
static StringArray std_include_paths;

char *base_file;
static char *output_file;
static StringArray input_paths;
char **chibicc_tmpfiles;

static const char kChibiccVersion[] = "\
chibicc (cosmopolitan) 9.0.0\n\
copyright 2019 rui ueyama\n\
copyright 2020 justine alexandra roberts tunney\n";

static void chibicc_version(void) {
  xwrite(1, kChibiccVersion, sizeof(kChibiccVersion) - 1);
  _Exit(0);
}

static void chibicc_usage(int status) {
  char *p;
  size_t n;
  p = xslurp("/zip/third_party/chibicc/help.txt", &n);
  __paginate(1, p);
  _Exit(status);
}

void chibicc_cleanup(void) {
  size_t i;
  if (chibicc_tmpfiles && !opt_save_temps) {
    for (i = 0; chibicc_tmpfiles[i]; i++) {
      unlink(chibicc_tmpfiles[i]);
    }
  }
}

static bool take_arg(char *arg) {
  char *x[] = {"-o", "-I",  "-idirafter", "-include",
               "-x", "-MF", "-MT",        "-Xlinker"};
  for (int i = 0; i < sizeof(x) / sizeof(*x); i++) {
    if (!strcmp(arg, x[i])) {
      return true;
    }
  }
  return false;
}

static void add_default_include_paths(char *argv0) {
  // We expect that chibicc-specific include files are installed
  // to ./include relative to argv[0].
  /* char *buf = calloc(1, strlen(argv0) + 10); */
  /* sprintf(buf, "%s/include", dirname(strdup(argv0))); */
  /* strarray_push(&include_paths, buf); */
  // Add standard include paths.
  /* strarray_push(&include_paths, "."); */
  strarray_push(&include_paths, "/zip/.c");
  // Keep a copy of the standard include paths for -MMD option.
  for (int i = 0; i < include_paths.len; i++) {
    strarray_push(&std_include_paths, include_paths.data[i]);
  }
}

static void define(char *str) {
  char *eq = strchr(str, '=');
  if (eq) {
    define_macro(strndup(str, eq - str), eq + 1);
  } else {
    define_macro(str, "1");
  }
}

static FileType parse_opt_x(char *s) {
  if (!strcmp(s, "c")) return FILE_C;
  if (!strcmp(s, "assembler")) return FILE_ASM;
  if (!strcmp(s, "assembler-with-cpp")) return FILE_ASM_CPP;
  if (!strcmp(s, "none")) return FILE_NONE;
  error("<command line>: unknown argument for -x: %s", s);
}

static char *quote_makefile(char *s) {
  char *buf = calloc(1, strlen(s) * 2 + 1);
  for (int i = 0, j = 0; s[i]; i++) {
    switch (s[i]) {
      case '$':
        buf[j++] = '$';
        buf[j++] = '$';
        break;
      case '#':
        buf[j++] = '\\';
        buf[j++] = '#';
        break;
      case ' ':
      case '\t':
        for (int k = i - 1; k >= 0 && s[k] == '\\'; k--) buf[j++] = '\\';
        buf[j++] = '\\';
        buf[j++] = s[i];
        break;
      default:
        buf[j++] = s[i];
        break;
    }
  }
  return buf;
}

static void PrintMemoryUsage(void) {
  struct mallinfo mi;
  malloc_trim(0);
  mi = mallinfo();
  fprintf(stderr, "\n");
  fprintf(stderr, "allocated %,ld bytes of memory\n", mi.arena);
  fprintf(stderr, "allocated %,ld nodes (%,ld bytes)\n", alloc_node_count,
          sizeof(Node) * alloc_node_count);
  fprintf(stderr, "allocated %,ld tokens (%,ld bytes)\n", alloc_token_count,
          sizeof(Token) * alloc_token_count);
  fprintf(stderr, "allocated %,ld objs (%,ld bytes)\n", alloc_obj_count,
          sizeof(Obj) * alloc_obj_count);
  fprintf(stderr, "allocated %,ld types (%,ld bytes)\n", alloc_type_count,
          sizeof(Type) * alloc_type_count);
  fprintf(stderr, "chibicc hashmap hits %,ld\n", chibicc_hashmap_hits);
  fprintf(stderr, "chibicc hashmap miss %,ld\n", chibicc_hashmap_miss);
  fprintf(stderr, "as hashmap hits %,ld\n", as_hashmap_hits);
  fprintf(stderr, "as hashmap miss %,ld\n", as_hashmap_miss);
}

static void strarray_push_comma(StringArray *a, char *s) {
  char *p;
  for (; *s++ == ','; s = p) {
    p = strchrnul(s, ',');
    strarray_push(a, strndup(s, p - s));
  }
}

static void parse_args(int argc, char **argv) {
  // Make sure that all command line options that take an argument
  // have an argument.
  for (int i = 1; i < argc; i++) {
    if (take_arg(argv[i])) {
      if (!argv[++i]) {
        chibicc_usage(1);
      }
    }
  }
  StringArray idirafter = {0};
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-###")) {
      opt_verbose = opt_hash_hash_hash = true;
    } else if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
    } else if (!strcmp(argv[i], "--help")) {
      chibicc_usage(0);
    } else if (!strcmp(argv[i], "--version")) {
      chibicc_version();
    } else if (!strcmp(argv[i], "-v")) {
      opt_verbose = true;
      atexit(PrintMemoryUsage);
    } else if (!strcmp(argv[i], "-o")) {
      opt_o = argv[++i];
    } else if (startswith(argv[i], "-o")) {
      opt_o = argv[i] + 2;
    } else if (!strcmp(argv[i], "-S")) {
      opt_S = true;
    } else if (!strcmp(argv[i], "-fcommon")) {
      opt_common = true;
    } else if (!strcmp(argv[i], "-fno-common")) {
      opt_common = false;
    } else if (!strcmp(argv[i], "-fno-builtin")) {
      opt_no_builtin = true;
    } else if (!strcmp(argv[i], "-save-temps")) {
      opt_save_temps = true;
    } else if (!strcmp(argv[i], "-c")) {
      opt_c = true;
    } else if (!strcmp(argv[i], "-E")) {
      opt_E = true;
    } else if (!strcmp(argv[i], "-J")) {
      opt_J = true;
    } else if (!strcmp(argv[i], "-A")) {
      opt_A = true;
    } else if (!strcmp(argv[i], "-P")) {
      opt_P = true;
    } else if (!strcmp(argv[i], "-I")) {
      strarray_push(&include_paths, argv[++i]);
    } else if (startswith(argv[i], "-I")) {
      strarray_push(&include_paths, argv[i] + 2);
    } else if (!strcmp(argv[i], "-iquote")) {
      strarray_push(&include_paths, argv[++i]);
    } else if (startswith(argv[i], "-iquote")) {
      strarray_push(&include_paths, argv[i] + strlen("-iquote"));
    } else if (!strcmp(argv[i], "-isystem")) {
      strarray_push(&include_paths, argv[++i]);
    } else if (startswith(argv[i], "-isystem")) {
      strarray_push(&include_paths, argv[i] + strlen("-isystem"));
    } else if (!strcmp(argv[i], "-D")) {
      define(argv[++i]);
    } else if (startswith(argv[i], "-D")) {
      define(argv[i] + 2);
    } else if (!strcmp(argv[i], "-U")) {
      undef_macro(argv[++i]);
    } else if (!strncmp(argv[i], "-U", 2)) {
      undef_macro(argv[i] + 2);
    } else if (!strcmp(argv[i], "-include")) {
      strarray_push(&opt_include, argv[++i]);
    } else if (!strcmp(argv[i], "-x")) {
      opt_x = parse_opt_x(argv[++i]);
    } else if (!strncmp(argv[i], "-x", 2)) {
      opt_x = parse_opt_x(argv[i] + 2);
    } else if (startswith(argv[i], "-Wa")) {
      strarray_push_comma(&as_extra_args, argv[i] + 3);
    } else if (startswith(argv[i], "-Wl")) {
      strarray_push_comma(&ld_extra_args, argv[i] + 3);
    } else if (!strcmp(argv[i], "-Xassembler")) {
      strarray_push(&as_extra_args, argv[++i]);
    } else if (!strcmp(argv[i], "-Xlinker")) {
      strarray_push(&ld_extra_args, argv[++i]);
    } else if (!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "-Wl,", 4)) {
      strarray_push(&input_paths, argv[i]);
    } else if (!strcmp(argv[i], "-s")) {
      strarray_push(&ld_extra_args, "-s");
    } else if (!strcmp(argv[i], "-M")) {
      opt_M = true;
    } else if (!strcmp(argv[i], "-MF")) {
      opt_MF = argv[++i];
    } else if (!strcmp(argv[i], "-MP")) {
      opt_MP = true;
    } else if (!strcmp(argv[i], "-MT")) {
      if (!opt_MT) {
        opt_MT = argv[++i];
      } else {
        opt_MT = xasprintf("%s %s", opt_MT, argv[++i]);
      }
    } else if (!strcmp(argv[i], "-MD")) {
      opt_MD = true;
    } else if (!strcmp(argv[i], "-MQ")) {
      if (!opt_MT) {
        opt_MT = quote_makefile(argv[++i]);
      } else {
        opt_MT = xasprintf("%s %s", opt_MT, quote_makefile(argv[++i]));
      }
    } else if (!strcmp(argv[i], "-MMD")) {
      opt_MD = opt_MMD = true;
    } else if (!strcmp(argv[i], "-fpie") || !strcmp(argv[i], "-fpic") ||
               !strcmp(argv[i], "-fPIC")) {
      opt_pic = true;
    } else if (!strcmp(argv[i], "-pg")) {
      opt_pg = true;
    } else if (!strcmp(argv[i], "-mfentry")) {
      opt_fentry = true;
    } else if (!strcmp(argv[i], "-ffunction-sections")) {
      opt_function_sections = true;
    } else if (!strcmp(argv[i], "-fdata-sections")) {
      opt_data_sections = true;
    } else if (!strcmp(argv[i], "-mrecord-mcount")) {
      opt_record_mcount = true;
    } else if (!strcmp(argv[i], "-mnop-mcount")) {
      opt_nop_mcount = true;
    } else if (!strcmp(argv[i], "-msse3")) {
      opt_sse3 = true;
    } else if (!strcmp(argv[i], "-msse4") || !strcmp(argv[i], "-msse4.2") ||
               !strcmp(argv[i], "-msse4.1")) {
      opt_sse4 = true;
    } else if (!strcmp(argv[i], "-mpopcnt")) {
      opt_popcnt = true;
    } else if (!strcmp(argv[i], "-cc1-input")) {
      base_file = argv[++i];
    } else if (!strcmp(argv[i], "-cc1-output")) {
      output_file = argv[++i];
    } else if (!strcmp(argv[i], "-idirafter")) {
      strarray_push(&idirafter, argv[i++]);
    } else if (!strcmp(argv[i], "-static")) {
      opt_static = true;
      strarray_push(&ld_extra_args, "-static");
    } else if (!strcmp(argv[i], "-shared")) {
      error("-shared not supported");
    } else if (!strcmp(argv[i], "-L")) {
      strarray_push(&ld_extra_args, "-L");
      strarray_push(&ld_extra_args, argv[++i]);
    } else if (startswith(argv[i], "-L")) {
      strarray_push(&ld_extra_args, "-L");
      strarray_push(&ld_extra_args, argv[i] + 2);
    } else {
      if (argv[i][0] == '-' && argv[i][1]) {
        /* compiler should not whine about the flags race */
        if (opt_verbose) {
          fprintf(stderr, "unknown argument: %s\n", argv[i]);
        }
      } else {
        strarray_push(&input_paths, argv[i]);
      }
    }
  }
  for (int i = 0; i < idirafter.len; i++) {
    strarray_push(&include_paths, idirafter.data[i]);
  }
  if (!input_paths.len) {
    error("no input files");
  }
  // -E implies that the input is the C macro language.
  if (opt_E) opt_x = FILE_C;
}

static FILE *open_file(char *path) {
  if (!path || strcmp(path, "-") == 0) return stdout;
  FILE *out = fopen(path, "w");
  if (!out) error("cannot open output file: %s: %s", path, strerror(errno));
  return out;
}

// Replace file extension
static char *replace_extn(char *tmpl, char *extn) {
  char *filename = basename(strdup(tmpl));
  int len1 = strlen(filename);
  int len2 = strlen(extn);
  char *buf = calloc(1, len1 + len2 + 2);
  char *dot = strrchr(filename, '.');
  if (dot) *dot = '\0';
  sprintf(buf, "%s%s", filename, extn);
  return buf;
}

static char *create_tmpfile(void) {
  char *path = xjoinpaths(__get_tmpdir(), "chibicc-XXXXXX");
  int fd = mkstemp(path);
  if (fd == -1) error("%s: mkstemp failed: %s", path, strerror(errno));
  close(fd);
  static int len = 2;
  chibicc_tmpfiles = realloc(chibicc_tmpfiles, sizeof(char *) * len);
  chibicc_tmpfiles[len - 2] = path;
  chibicc_tmpfiles[len - 1] = NULL;
  len++;
  return path;
}

static void handle_exit(bool ok) {
  if (!ok) {
    opt_save_temps = true;
    exit(1);
  }
}

static bool NeedsShellQuotes(const char *s) {
  if (*s) {
    for (;;) {
      switch (*s++ & 255) {
        case 0:
          return false;
        case '-':
        case '.':
        case '/':
        case '_':
        case '0' ... '9':
        case 'A' ... 'Z':
        case 'a' ... 'z':
          break;
        default:
          return true;
      }
    }
  } else {
    return true;
  }
}

static bool run_subprocess(char **argv) {
  int rc, ws;
  size_t i, j;
  if (opt_verbose) {
    for (i = 0; argv[i]; i++) {
      fputc(' ', stderr);
      if (opt_hash_hash_hash && NeedsShellQuotes(argv[i])) {
        fputc('\'', stderr);
        for (j = 0; argv[i][j]; ++j) {
          if (argv[i][j] != '\'') {
            fputc(argv[i][j], stderr);
          } else {
            fputs("'\"'\"'", stderr);
          }
        }
        fputc('\'', stderr);
      } else {
        fputs(argv[i], stderr);
      }
    }
    fputc('\n', stderr);
  }
  if (!vfork()) {
    // Child process. Run a new command.
    execvp(argv[0], argv);
    _Exit(1);
  }
  // Wait for the child process to finish.
  do rc = wait(&ws);
  while (rc == -1 && errno == EINTR);
  return WIFEXITED(ws) && WEXITSTATUS(ws) == 0;
}

static bool run_cc1(int argc, char **argv, char *input, char *output) {
  char **args = calloc(argc + 10, sizeof(char *));
  memcpy(args, argv, argc * sizeof(char *));
  args[argc++] = "-cc1";
  if (input) {
    args[argc++] = "-cc1-input";
    args[argc++] = input;
  }
  if (output) {
    args[argc++] = "-cc1-output";
    args[argc++] = output;
  }
  return run_subprocess(args);
}

static void print_token(FILE *out, Token *tok) {
  switch (tok->kind) {
    case TK_STR:
      switch (tok->ty->base->size) {
        case 1:
          fprintf(out, "%`'.*s", tok->ty->array_len - 1, tok->str);
          break;
        case 2:
          fprintf(out, "%`'.*hs", tok->ty->array_len - 1, tok->str);
          break;
        case 4:
          fprintf(out, "%`'.*ls", tok->ty->array_len - 1, tok->str);
          break;
        default:
          UNREACHABLE();
      }
      break;
    default:
      fprintf(out, "%.*s", tok->len, tok->loc);
      break;
  }
}

static void print_tokens(Token *tok) {
  FILE *out = open_file(opt_o ? opt_o : "-");
  int line = 1;
  for (; tok->kind != TK_EOF; tok = tok->next) {
    if (line > 1 && tok->at_bol) fprintf(out, "\n");
    if (tok->has_space && !tok->at_bol) fprintf(out, " ");
    print_token(out, tok);
    line++;
  }
  fprintf(out, "\n");
}

static bool in_std_include_path(char *path) {
  for (int i = 0; i < std_include_paths.len; i++) {
    char *dir = std_include_paths.data[i];
    int len = strlen(dir);
    if (strncmp(dir, path, len) == 0 && path[len] == '/') return true;
  }
  return false;
}

// If -M options is given, the compiler write a list of input files to
// stdout in a format that "make" command can read. This feature is
// used to automate file dependency management.
static void print_dependencies(void) {
  char *path;
  if (opt_MF) {
    path = opt_MF;
  } else if (opt_MD) {
    path = replace_extn(opt_o ? opt_o : base_file, ".d");
  } else if (opt_o) {
    path = opt_o;
  } else {
    path = "-";
  }
  FILE *out = open_file(path);
  if (opt_MT)
    fprintf(out, "%s:", opt_MT);
  else
    fprintf(out, "%s:", quote_makefile(replace_extn(base_file, ".o")));
  File **files = get_input_files();
  for (int i = 0; files[i]; i++) {
    if (opt_MMD && in_std_include_path(files[i]->name)) continue;
    fprintf(out, " \\\n\t%s", files[i]->name);
  }
  fprintf(out, "\n\n");
  if (opt_MP) {
    for (int i = 1; files[i]; i++) {
      if (opt_MMD && in_std_include_path(files[i]->name)) continue;
      fprintf(out, "%s:\n\n", quote_makefile(files[i]->name));
    }
  }
}

static Token *must_tokenize_file(char *path) {
  Token *tok = tokenize_file(path);
  if (!tok) error("%s: %s", path, strerror(errno));
  return tok;
}

static Token *append_tokens(Token *tok1, Token *tok2) {
  if (!tok1 || tok1->kind == TK_EOF) return tok2;
  Token *t = tok1;
  while (t->next->kind != TK_EOF) t = t->next;
  t->next = tok2;
  return tok1;
}

static FileType get_file_type(const char *filename) {
  if (opt_x != FILE_NONE) return opt_x;
  if (endswith(filename, ".a")) return FILE_AR;
  if (endswith(filename, ".o")) return FILE_OBJ;
  if (endswith(filename, ".c")) return FILE_C;
  if (endswith(filename, ".s")) return FILE_ASM;
  if (endswith(filename, ".S")) return FILE_ASM_CPP;
  error("<command line>: unknown file extension: %s", filename);
}

static void cc1(void) {
  FileType ft;
  Token *tok = NULL;
  ft = get_file_type(base_file);
  if (opt_J && (ft == FILE_ASM || ft == FILE_ASM_CPP)) {
    output_javadown_asm(output_file, base_file);
    return;
  }
  // Process -include option
  for (int i = 0; i < opt_include.len; i++) {
    char *incl = opt_include.data[i];
    char *path;
    if (fileexists(incl)) {
      path = incl;
    } else {
      path = search_include_paths(incl);
      if (!path) error("-include: %s: %s", incl, strerror(errno));
    }
    Token *tok2 = must_tokenize_file(path);
    tok = append_tokens(tok, tok2);
  }
  // Tokenize and parse.
  Token *tok2 = must_tokenize_file(base_file);
  tok = append_tokens(tok, tok2);
  tok = preprocess(tok);
  // If -M or -MD are given, print file dependencies.
  if (opt_M || opt_MD) {
    print_dependencies();
    if (opt_M) return;
  }
  // If -E is given, print out preprocessed C code as a result.
  if (opt_E || ft == FILE_ASM_CPP) {
    print_tokens(tok);
    return;
  }
  Obj *prog = parse(tok);
  if (opt_A) {
    print_ast(stdout, prog);
    return;
  }
  if (opt_J) {
    output_javadown(output_file, prog);
    return;
  }
  if (opt_P) {
    output_bindings_python(output_file, prog, tok2);
    return;
  }
  FILE *out = open_file(output_file);
  codegen(prog, out);
  fclose(out);
}

static int CountArgv(char **argv) {
  int n = 0;
  while (*argv++) ++n;
  return n;
}

static void assemble(char *input, char *output) {
  char *as = getenv("AS");
  if (!as || !*as) as = "as";
  StringArray arr = {0};
  strarray_push(&arr, as);
  strarray_push(&arr, "-W");
  strarray_push(&arr, "-I.");
  strarray_push(&arr, "-c");
  for (int i = 0; i < as_extra_args.len; i++) {
    strarray_push(&arr, as_extra_args.data[i]);
  }
  strarray_push(&arr, input);
  strarray_push(&arr, "-o");
  strarray_push(&arr, output);
  if (1) {
    bool kludge = opt_save_temps;
    opt_save_temps = true;
    Assembler(CountArgv(arr.data), arr.data);
    opt_save_temps = kludge;
  } else {
    handle_exit(run_subprocess(arr.data));
  }
}

static void run_linker(StringArray *inputs, char *output) {
  char *ld = getenv("LD");
  if (!ld || !*ld) ld = "ld";
  StringArray arr = {0};
  strarray_push(&arr, ld);
  strarray_push(&arr, "-m");
  strarray_push(&arr, "elf_x86_64");
  strarray_push(&arr, "-z");
  strarray_push(&arr, "max-page-size=0x1000");
  strarray_push(&arr, "-static");
  strarray_push(&arr, "-nostdlib");
  strarray_push(&arr, "--gc-sections");
  strarray_push(&arr, "--build-id=none");
  strarray_push(&arr, "--no-dynamic-linker");
  /* strarray_push(&arr, "-T"); */
  /* strarray_push(&arr, LDS); */
  /* strarray_push(&arr, APE); */
  /* strarray_push(&arr, CRT); */
  for (int i = 0; i < ld_extra_args.len; i++) {
    strarray_push(&arr, ld_extra_args.data[i]);
  }
  for (int i = 0; i < inputs->len; i++) {
    strarray_push(&arr, inputs->data[i]);
  }
  strarray_push(&arr, "-o");
  strarray_push(&arr, output);
  handle_exit(run_subprocess(arr.data));
}

static void OnCtrlC(int sig, siginfo_t *si, void *ctx) {
  exit(1);
}

int chibicc(int argc, char **argv) {
#ifndef NDEBUG
  ShowCrashReports();
#endif
  atexit(chibicc_cleanup);
  sigaction(SIGINT, &(struct sigaction){.sa_sigaction = OnCtrlC}, NULL);
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
      break;
    }
  }
  if (opt_cc1) init_macros();
  parse_args(argc, argv);
  if (opt_cc1) {
    init_macros_conditional();
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }
  if (input_paths.len > 1 && opt_o && (opt_c || opt_S | opt_E)) {
    error("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");
  }
  StringArray ld_args = {0};
  StringArray dox_args = {0};
  for (int i = 0; i < input_paths.len; i++) {
    char *input = input_paths.data[i];
    if (!strncmp(input, "-l", 2)) {
      strarray_push(&ld_args, input);
      continue;
    }
    if (!strncmp(input, "-Wl,", 4)) {
      char *s = strdup(input + 4);
      char *arg = strtok(s, ",");
      while (arg) {
        strarray_push(&ld_args, arg);
        arg = strtok(NULL, ",");
      }
      continue;
    }
    char *output;
    if (opt_o) {
      output = opt_o;
    } else if (opt_S) {
      output = replace_extn(input, ".s");
    } else {
      output = replace_extn(input, ".o");
    }
    FileType type = get_file_type(input);
    // Handle .o or .a
    if (type == FILE_OBJ || type == FILE_AR || type == FILE_DSO) {
      strarray_push(&ld_args, input);
      continue;
    }
    // Dox
    if (opt_J) {
      if (opt_c) {
        handle_exit(run_cc1(argc, argv, input, output));
      } else {
        char *tmp = create_tmpfile();
        if (run_cc1(argc, argv, input, tmp)) {
          strarray_push(&dox_args, tmp);
        }
      }
      continue;
    }
    // Handle .s
    if (type == FILE_ASM) {
      if (!opt_S) {
        assemble(input, output);
      }
      continue;
    }
    assert(type == FILE_C || type == FILE_ASM_CPP);
    // Just print ast.
    if (opt_A) {
      handle_exit(run_cc1(argc, argv, input, NULL));
      continue;
    }
    // Just preprocess
    if (opt_E || opt_M) {
      handle_exit(run_cc1(argc, argv, input, NULL));
      continue;
    }
    // Python Bindings
    if (opt_P) {
      handle_exit(run_cc1(argc, argv, input, opt_o ? opt_o : "/dev/stdout"));
      continue;
    }
    // Compile
    if (opt_S) {
      handle_exit(run_cc1(argc, argv, input, output));
      continue;
    }
    // Compile and assemble
    if (opt_c) {
      char *tmp = create_tmpfile();
      handle_exit(run_cc1(argc, argv, input, tmp));
      assemble(tmp, output);
      continue;
    }
    // Compile, assemble and link
    char *tmp1 = create_tmpfile();
    char *tmp2 = create_tmpfile();
    handle_exit(run_cc1(argc, argv, input, tmp1));
    assemble(tmp1, tmp2);
    strarray_push(&ld_args, tmp2);
    continue;
  }
  if (ld_args.len > 0) {
    run_linker(&ld_args, opt_o ? opt_o : "a.out");
  }
  if (dox_args.len > 0) {
    drop_dox(&dox_args, opt_o ? opt_o : "/dev/stdout");
  }
  return 0;
}
