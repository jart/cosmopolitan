/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmo.h"
#include "libc/ctype.h"
#include "libc/cxxabi.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/tab.h"
#include "libc/sysv/consts/o.h"

__static_yoink("zipos");

#define CLEAR(b)                                        \
  do {                                                  \
    b##_cap = 8;                                        \
    b##_len = 0;                                        \
    if (!(b##_ptr = calloc(b##_cap, sizeof(*b##_ptr)))) \
      goto Failure;                                     \
  } while (0)

#define APPEND(b, c)                                           \
  do {                                                         \
    if (b##_len + 2 > b##_cap) {                               \
      b##_cap += b##_cap >> 1;                                 \
      void *p_ = realloc(b##_ptr, b##_cap * sizeof(*b##_ptr)); \
      if (!p_)                                                 \
        goto Failure;                                          \
      b##_ptr = p_;                                            \
    }                                                          \
    b##_ptr[b##_len++] = c;                                    \
    b##_ptr[b##_len] = 0;                                      \
  } while (0)

#define APPEND_DUP(b, s)    \
  do {                      \
    char *tmp;              \
    if (!(tmp = strdup(s))) \
      goto Failure;         \
    APPEND(args, tmp);      \
  } while (0)

static int esc(int c) {
  switch (c) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 't':
      return '\t';
    case 'n':
      return '\n';
    case 'v':
      return '\v';
    case 'f':
      return '\f';
    case 'r':
      return '\r';
    case 'e':
      return '\e';
    default:
      return -1;
  }
}

static void cosmo_args_free(void *list) {
  char **args = list;
  char *arg;
  while ((arg = *args++))
    free(arg);
  free(list);
}

/**
 * Replaces argument list with `/zip/.args` contents if it exists.
 *
 * First read the documentation to LoadZipArgs(). This works basically
 * the same, assuming you pass `"/zip/.args"` as the first argument. The
 * difference is that arguments here are parsed more similarly to the
 * shell. In the old version, if you wanted your zip .args config to
 * insert three arguments at the beginning of your argv, you'd say:
 *
 *     arg1
 *     arg2
 *     arg3
 *
 * This will still work. You can also now say:
 *
 *     arg1 arg2
 *     arg3
 *
 * This breaks backwards compatibility, since the old design was made
 * for programs like ar.ape that wanted to be able to accept filename
 * arguments that could potentially have spaces. This new parser, on the
 * other hand, is designed to help offer the configurability a project
 * like llamafile needs, without going so far as to be Turing complete.
 * For example, you could say:
 *
 *     # this is a comment
 *     this\ is' a single arg'"ok"# # comment
 *
 * Which will result in the C string `"this is a single argok#"`. You
 * can even use $VAR notation to schlep in environment variables. Here's
 * how this is different from shell:
 *
 * 1. We don't expand $foo into multiple arguments if it has spaces
 * 2. Double quoted strings work like they do in C, e.g. `"\177\x7f\n"`
 * 3. You can't recursively reference environment variables
 *
 * If the process was started in a degenerate state without argv[0] then
 * GetProgramExecutableName() will be inserted in its place, on success.
 *
 * The `path` argument may be null, in which case only normalization is
 * performed. It is not considered an error if `path` is specified and
 * the file doesn't exist. The errno state will be left dirty if that
 * happens, so it can be checked by clearing `errno` before calling.
 *
 * The returned memory is copied and automatically freed on exit().
 *
 * @return argc on success, or -1 w/ errno
 */
int cosmo_args(const char *path, char ***argv) {

  // the file
  int fd = -1;

  // variable name builder
  int var_cap = 0;
  int var_len = 0;
  char *var_ptr = 0;

  // argument string builder
  int arg_cap = 0;
  int arg_len = 0;
  char *arg_ptr = 0;

  // argument array builder
  int args_cap = 0;
  int args_len = 0;
  char **args_ptr = 0;

  // initialize memory
  CLEAR(var);
  CLEAR(arg);
  CLEAR(args);

  // state machine
  enum {
    NORMAL,
    COMMENT,
    ARGUMENT,
    BACKSLASH,
    DOLLAR,
    DOLLAR_VAR,
    DOLLAR_LCB,
    DOT,
    DOT_DOT,
    DOT_DOT_DOT,
    QUOTE,
    DQUOTE,
    DQUOTE_DOLLAR,
    DQUOTE_DOLLAR_VAR,
    DQUOTE_DOLLAR_LCB,
    DQUOTE_BACKSLASH,
    DQUOTE_BACKSLASH_X,
    DQUOTE_BACKSLASH_X_XDIGIT,
    DQUOTE_BACKSLASH_DIGIT,
    DQUOTE_BACKSLASH_DIGIT_DIGIT,
  } t = NORMAL;

  // extra state
  int x, numba = 0;

  // add program argument
  char **argvp = *argv;
  if (*argvp) {
    APPEND_DUP(args, *argvp++);
  } else {
    APPEND_DUP(args, GetProgramExecutableName());
  }

  // perform i/o
  if (path) {
    if ((fd = open(path, O_RDONLY)) == -1)
      if (errno != ENOENT && errno != ENOTDIR)
        goto Failure;
    if (fd != -1) {
      for (;;) {
        char buf[512];
        int got = read(fd, buf, sizeof(buf));
        if (got == -1)
          goto Failure;
        if (!got)
          break;
        for (int i = 0; i < got; ++i) {
          int c = buf[i] & 255;
          switch (t) {

            case NORMAL:
              switch (c) {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                case '\f':
                case '\v':
                  break;
                case '#':
                  t = COMMENT;
                  break;
                case '\'':
                  t = QUOTE;
                  break;
                case '"':
                  t = DQUOTE;
                  break;
                case '$':
                  t = DOLLAR;
                  break;
                case '.':
                  t = DOT;
                  break;
                case '\\':
                  t = BACKSLASH;
                  break;
                default:
                  APPEND(arg, c);
                  t = ARGUMENT;
                  break;
              }
              break;

            Argument:
            case ARGUMENT:
              switch (c) {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                case '\f':
                case '\v':
                  APPEND(args, arg_ptr);
                  CLEAR(arg);
                  t = NORMAL;
                  break;
                case '\'':
                  t = QUOTE;
                  break;
                case '"':
                  t = DQUOTE;
                  break;
                case '$':
                  t = DOLLAR;
                  break;
                case '\\':
                  t = BACKSLASH;
                  break;
                default:
                  APPEND(arg, c);
                  break;
              }
              break;

            case BACKSLASH:
              if (c == '\r') {
                // do nothing
              } else if (c == '\n') {
                t = NORMAL;
              } else if ((x = esc(c)) != -1) {
                APPEND(arg, x);
                t = ARGUMENT;
              } else {
                APPEND(arg, c);
                t = ARGUMENT;
              }
              break;

            case COMMENT:
              if (c == '\n')
                t = NORMAL;
              break;

            case DOLLAR:
              if (isalnum(c) || c == '_') {
                APPEND(var, c);
                t = DOLLAR_VAR;
              } else if (c == '{') {
                t = DOLLAR_LCB;
              } else {
                APPEND(arg, '$');
                if (c != '$') {
                  t = ARGUMENT;
                  goto Argument;
                }
              }
              break;

            case DOLLAR_VAR:
              if (isalnum(c) || c == '_') {
                APPEND(var, c);
              } else {
                char *val = getenv(var_ptr);
                if (!val)
                  val = "";
                free(var_ptr);
                CLEAR(var);
                while (*val)
                  APPEND(arg, *val++);
                t = ARGUMENT;
                goto Argument;
              }
              break;

            case DOLLAR_LCB:
              if (c == '}') {
                char *val = getenv(var_ptr);
                if (!val)
                  val = "";
                free(var_ptr);
                CLEAR(var);
                while (*val)
                  APPEND(arg, *val++);
                t = ARGUMENT;
              } else {
                APPEND(var, c);
              }
              break;

            case QUOTE:
              if (c == '\'') {
                t = ARGUMENT;
              } else {
                APPEND(arg, c);
              }
              break;

            Dquote:
            case DQUOTE:
              if (c == '"') {
                t = ARGUMENT;
              } else if (c == '$') {
                t = DQUOTE_DOLLAR;
              } else if (c == '\\') {
                t = DQUOTE_BACKSLASH;
              } else {
                APPEND(arg, c);
              }
              break;

            case DQUOTE_DOLLAR:
              if (isalnum(c) || c == '_') {
                APPEND(var, c);
                t = DQUOTE_DOLLAR_VAR;
              } else if (c == '{') {
                t = DQUOTE_DOLLAR_LCB;
              } else {
                APPEND(arg, '$');
                if (c != '$') {
                  t = DQUOTE;
                  goto Dquote;
                }
              }
              break;

            case DQUOTE_DOLLAR_VAR:
              if (isalnum(c) || c == '_') {
                APPEND(var, c);
              } else {
                char *val = getenv(var_ptr);
                if (!val)
                  val = "";
                free(var_ptr);
                CLEAR(var);
                while (*val)
                  APPEND(arg, *val++);
                t = DQUOTE;
                goto Dquote;
              }
              break;

            case DQUOTE_DOLLAR_LCB:
              if (c == '}') {
                char *val = getenv(var_ptr);
                if (!val)
                  val = "";
                free(var_ptr);
                CLEAR(var);
                while (*val)
                  APPEND(arg, *val++);
                t = DQUOTE;
              } else {
                APPEND(var, c);
              }
              break;

            case DQUOTE_BACKSLASH:
              if (isdigit(c)) {
                numba = c - '0';
                t = DQUOTE_BACKSLASH_DIGIT;
              } else if (c == 'x') {
                t = DQUOTE_BACKSLASH_X;
              } else if ((x = esc(c)) != -1) {
                APPEND(arg, x);
                t = DQUOTE;
              } else if (c == '\r') {
                // do nothing
              } else if (c == '\n') {
                t = DQUOTE;
              } else {
                APPEND(arg, c);
                t = DQUOTE;
              }
              break;

            case DQUOTE_BACKSLASH_DIGIT:
              if (isdigit(c)) {
                numba <<= 3;
                numba += c - '0';
                t = DQUOTE_BACKSLASH_DIGIT_DIGIT;
              } else {
                APPEND(arg, numba);
                t = DQUOTE;
                goto Dquote;
              }
              break;

            case DQUOTE_BACKSLASH_DIGIT_DIGIT:
              if (isdigit(c)) {
                numba <<= 3;
                numba += c - '0';
                APPEND(arg, numba);
                t = DQUOTE;
              } else {
                APPEND(arg, numba);
                t = DQUOTE;
                goto Dquote;
              }
              break;

            case DQUOTE_BACKSLASH_X:
              if ((x = kHexToInt[c]) != -1) {
                numba = x;
                t = DQUOTE_BACKSLASH_X_XDIGIT;
              } else {
                APPEND(arg, 'x');
                t = DQUOTE;
                goto Dquote;
              }
              break;

            case DQUOTE_BACKSLASH_X_XDIGIT:
              if ((x = kHexToInt[c]) != -1) {
                numba <<= 4;
                numba += x;
                APPEND(arg, numba);
                t = DQUOTE;
              } else {
                APPEND(arg, numba);
                t = DQUOTE;
                goto Dquote;
              }
              break;

            case DOT:
              if (c == '.') {
                t = DOT_DOT;
              } else {
                APPEND(arg, '.');
                t = ARGUMENT;
                goto Argument;
              }
              break;

            case DOT_DOT:
              if (c == '.') {
                t = DOT_DOT_DOT;
              } else {
                APPEND(arg, '.');
                APPEND(arg, '.');
                t = ARGUMENT;
                goto Argument;
              }
              break;

            case DOT_DOT_DOT:
              if (isspace(c)) {
                while (*argvp)
                  APPEND_DUP(args, *argvp++);
                t = NORMAL;
              } else {
                APPEND(arg, '.');
                APPEND(arg, '.');
                APPEND(arg, '.');
                t = ARGUMENT;
                goto Argument;
              }
              break;

            default:
              __builtin_unreachable();
          }
        }
      }
      if (close(fd))
        goto Failure;

      // clean up dirty state
      switch (t) {
        case DOT:
          APPEND(arg, '.');
          break;
        case DOT_DOT:
          APPEND(arg, '.');
          APPEND(arg, '.');
          break;
        case DOT_DOT_DOT:
          while (*argvp)
            APPEND_DUP(args, *argvp++);
          break;
        case DOLLAR:
          APPEND(arg, '$');
          break;
        case DOLLAR_VAR:
        case DQUOTE_DOLLAR_VAR:
          char *val = getenv(var_ptr);
          if (!val)
            val = "";
          while (*val)
            APPEND(arg, *val++);
          break;
        case DOLLAR_LCB:
        case DQUOTE_DOLLAR_LCB:
          APPEND(arg, '$');
          APPEND(arg, '{');
          for (int j = 0; var_ptr[j]; ++j)
            APPEND(arg, var_ptr[j]);
          break;
        default:
          break;
      }
      if (arg_len) {
        APPEND(args, arg_ptr);
        CLEAR(arg);
      }
    }
  }

  // append original argv if ... wasn't specified
  while (*argvp)
    APPEND_DUP(args, *argvp++);

  // return result
  __cxa_atexit(cosmo_args_free, args_ptr, 0);
  *argv = args_ptr;
  free(arg_ptr);
  free(var_ptr);
  return args_len;

Failure:
  cosmo_args_free(args_ptr);
  free(arg_ptr);
  if (fd != -1)
    close(fd);
  return -1;
}
