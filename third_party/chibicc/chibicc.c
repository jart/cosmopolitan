#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "third_party/chibicc/chibicc.h"

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
static char **tmpfiles;

static void usage(int status) {
  fprintf(stderr, "chibicc [ -o <path> ] <file>\n");
  exit(status);
}

static void version(void) {
  printf("\
chibicc (cosmopolitan) 9.0.0\n\
copyright 2019 rui ueyama\n\
copyright 2020 justine alexandra roberts tunney\n");
  exit(0);
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
        usage(1);
      }
    }
  }
  StringArray idirafter = {};
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-###")) {
      opt_hash_hash_hash = true;
    } else if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
    } else if (!strcmp(argv[i], "--help")) {
      usage(0);
    } else if (!strcmp(argv[i], "--version")) {
      version();
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
    } else if (!strcmp(argv[i], "-c")) {
      opt_c = true;
    } else if (!strcmp(argv[i], "-E")) {
      opt_E = true;
    } else if (!strcmp(argv[i], "-J")) {
      opt_J = true;
    } else if (!strcmp(argv[i], "-A")) {
      opt_A = true;
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
      strarray_push_comma(&as_extra_args, argv[i]);
    } else if (startswith(argv[i], "-Wl")) {
      strarray_push_comma(&ld_extra_args, argv[i]);
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

static void cleanup(void) {
  if (tmpfiles && !opt_save_temps) {
    for (int i = 0; tmpfiles[i]; i++) {
      unlink(tmpfiles[i]);
    }
  }
}

static char *create_tmpfile(void) {
  char *path = xjoinpaths(kTmpPath, "chibicc-XXXXXX");
  int fd = mkstemp(path);
  if (fd == -1) error("mkstemp failed: %s", strerror(errno));
  close(fd);
  static int len = 2;
  tmpfiles = realloc(tmpfiles, sizeof(char *) * len);
  tmpfiles[len - 2] = path;
  tmpfiles[len - 1] = NULL;
  len++;
  return path;
}

static void handle_exit(bool ok) {
  if (!ok) {
    opt_save_temps = true;
    exit(1);
  }
}

static bool run_subprocess(char **argv) {
  // If -### is given, dump the subprocess's command line.
  if (opt_hash_hash_hash) {
    fprintf(stderr, "%s", argv[0]);
    for (int i = 1; argv[i]; i++) fprintf(stderr, " %s", argv[i]);
    fprintf(stderr, "\n");
  }
  if (!vfork()) {
    // Child process. Run a new command.
    execvp(argv[0], argv);
    _exit(1);
  }
  // Wait for the child process to finish.
  int status;
  for (;;) {
    if (wait(&status) <= 0) {
      break;
    }
  }
  return !status;
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
    fprintf(out, " \\\n  %s", files[i]->name);
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
  StringArray arr = {};
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
  strarray_push(&arr, NULL);
  if (1) {
    Assembler(CountArgv(arr.data), arr.data);
  } else {
    handle_exit(run_subprocess(arr.data));
  }
}

static void run_linker(StringArray *inputs, char *output) {
  char *ld = getenv("LD");
  if (!ld || !*ld) ld = "ld";
  StringArray arr = {};
  strarray_push(&arr, ld);
  strarray_push(&arr, "-o");
  strarray_push(&arr, output);
  strarray_push(&arr, "-m");
  strarray_push(&arr, "elf_x86_64");
  strarray_push(&arr, "-z");
  strarray_push(&arr, "max-page-size=0x1000");
  strarray_push(&arr, "-nostdlib");
  strarray_push(&arr, "--gc-sections");
  strarray_push(&arr, "--build-id=none");
  strarray_push(&arr, "--no-dynamic-linker");
  strarray_push(&arr, "-Ttext-segment=0x400000");
  strarray_push(&arr, "-T");
  strarray_push(&arr, LDS);
  strarray_push(&arr, APE);
  strarray_push(&arr, CRT);
  for (int i = 0; i < ld_extra_args.len; i++) {
    strarray_push(&arr, ld_extra_args.data[i]);
  }
  for (int i = 0; i < inputs->len; i++) {
    strarray_push(&arr, inputs->data[i]);
  }
  strarray_push(&arr, NULL);
  handle_exit(run_subprocess(arr.data));
}

static void OnCtrlC(int sig, siginfo_t *si, ucontext_t *ctx) {
  exit(1);
}

int chibicc(int argc, char **argv) {
  showcrashreports();
  sigaction(SIGINT, &(struct sigaction){.sa_sigaction = OnCtrlC}, NULL);
  atexit(cleanup);
  init_macros();
  parse_args(argc, argv);
  if (opt_cc1) {
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }
  if (input_paths.len > 1 && opt_o && (opt_c || opt_S | opt_E)) {
    error("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");
  }
  StringArray ld_args = {};
  StringArray dox_args = {};
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
