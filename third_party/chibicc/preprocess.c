// This file implements the C preprocessor.
//
// The preprocessor takes a list of tokens as an input and returns a
// new list of tokens as an output.
//
// The preprocessing language is designed in such a way that that's
// guaranteed to stop even if there is a recursive macro.
// Informally speaking, a macro is applied only once for each token.
// That is, if a macro token T appears in a result of direct or
// indirect macro expansion of T, T won't be expanded any further.
// For example, if T is defined as U, and U is defined as T, then
// token T is expanded to U and then to T and the macro expansion
// stops at that point.
//
// To achieve the above behavior, we attach for each token a set of
// macro names from which the token is expanded. The set is called
// "hideset". Hideset is initially empty, and every time we expand a
// macro, the macro name is added to the resulting tokens' hidesets.
//
// The above macro expansion algorithm is explained in this document
// written by Dave Prossor, which is used as a basis for the
// standard's wording:
// https://github.com/rui314/chibicc/wiki/cpp.algo.pdf

#include "libc/fmt/libgen.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/xasprintf.h"
#include "third_party/chibicc/chibicc.h"
#include "third_party/chibicc/kw.h"

typedef struct CondIncl CondIncl;
typedef struct Hideset Hideset;
typedef struct MacroArg MacroArg;

typedef enum {
  STR_NONE,
  STR_UTF8,
  STR_UTF16,
  STR_UTF32,
  STR_WIDE,
} StringKind;

struct MacroArg {
  MacroArg *next;
  char *name;
  bool is_va_args;
  Token *tok;
};

// `#if` can be nested, so we use a stack to manage nested `#if`s.
struct CondIncl {
  CondIncl *next;
  enum { IN_THEN, IN_ELIF, IN_ELSE } ctx;
  Token *tok;
  bool included;
};

struct Hideset {
  Hideset *next;
  char *name;
};

HashMap macros;

static CondIncl *cond_incl;
static HashMap pragma_once;
static int include_next_idx;

static Token *preprocess2(Token *);
static Macro *find_macro(Token *);

static inline bool is_hash(Token *tok) {
  return tok->at_bol && tok->len == 1 && tok->loc[0] == '#';
}

// Some preprocessor directives such as #include allow extraneous
// tokens before newline. This function skips such tokens.
static Token *skip_line(Token *tok) {
  if (tok->at_bol) return tok;
  warn_tok(tok, "extra token");
  while (tok->at_bol) tok = tok->next;
  return tok;
}

static Token *copy_token(Token *tok) {
  Token *t = alloc_token();
  *t = *tok;
  t->next = NULL;
  return t;
}

static Token *new_eof(Token *tok) {
  Token *t = copy_token(tok);
  t->kind = TK_EOF;
  t->len = 0;
  return t;
}

static Hideset *new_hideset(char *name) {
  Hideset *hs = calloc(1, sizeof(Hideset));
  hs->name = name;
  return hs;
}

static Hideset *hideset_union(Hideset *hs1, Hideset *hs2) {
  Hideset head = {};
  Hideset *cur = &head;
  for (; hs1; hs1 = hs1->next) {
    cur = cur->next = new_hideset(hs1->name);
  }
  cur->next = hs2;
  return head.next;
}

static bool hideset_contains(Hideset *hs, char *s, int len) {
  for (; hs; hs = hs->next) {
    if (strlen(hs->name) == len && !strncmp(hs->name, s, len)) {
      return true;
    }
  }
  return false;
}

static Hideset *hideset_intersection(Hideset *hs1, Hideset *hs2) {
  Hideset head = {};
  Hideset *cur = &head;
  for (; hs1; hs1 = hs1->next) {
    if (hideset_contains(hs2, hs1->name, strlen(hs1->name))) {
      cur = cur->next = new_hideset(hs1->name);
    }
  }
  return head.next;
}

static Token *add_hideset(Token *tok, Hideset *hs) {
  Token head = {};
  Token *cur = &head;
  for (; tok; tok = tok->next) {
    Token *t = copy_token(tok);
    t->hideset = hideset_union(t->hideset, hs);
    cur = cur->next = t;
  }
  return head.next;
}

// Append tok2 to the end of tok1.
static Token *append(Token *tok1, Token *tok2) {
  if (tok1->kind == TK_EOF) return tok2;
  Token head = {};
  Token *cur = &head;
  for (; tok1->kind != TK_EOF; tok1 = tok1->next)
    cur = cur->next = copy_token(tok1);
  cur->next = tok2;
  return head.next;
}

static Token *skip_cond_incl2(Token *tok) {
  unsigned char kw;
  while (tok->kind != TK_EOF) {
    if (is_hash(tok) && (kw = GetKw(tok->next->loc, tok->next->len))) {
      if (kw == KW_IF || kw == KW_IFDEF || kw == KW_IFNDEF) {
        tok = skip_cond_incl2(tok->next->next);
        continue;
      }
      if (kw == KW_ENDIF) {
        return tok->next->next;
      }
    }
    tok = tok->next;
  }
  return tok;
}

// Skip until next `#else`, `#elif` or `#endif`.
// Nested `#if` and `#endif` are skipped.
static Token *skip_cond_incl(Token *tok) {
  unsigned char kw;
  while (tok->kind != TK_EOF) {
    if (is_hash(tok) && (kw = GetKw(tok->next->loc, tok->next->len))) {
      if (kw == KW_IF || kw == KW_IFDEF || kw == KW_IFNDEF) {
        tok = skip_cond_incl2(tok->next->next);
        continue;
      }
      if (kw == KW_ELIF || kw == KW_ELSE || kw == KW_ENDIF) {
        break;
      }
    }
    tok = tok->next;
  }
  return tok;
}

// Double-quote a given string and returns it.
static char *quote_string(char *str) {
  int bufsize = 3;
  for (int i = 0; str[i]; i++) {
    if (str[i] == '\\' || str[i] == '"') bufsize++;
    bufsize++;
  }
  char *buf = calloc(1, bufsize);
  char *p = buf;
  *p++ = '"';
  for (int i = 0; str[i]; i++) {
    if (str[i] == '\\' || str[i] == '"') *p++ = '\\';
    *p++ = str[i];
  }
  *p++ = '"';
  *p++ = '\0';
  return buf;
}

static Token *new_str_token(char *str, Token *tmpl) {
  char *buf = quote_string(str);
  return tokenize(new_file(tmpl->file->name, tmpl->file->file_no, buf));
}

// Copy all tokens until the next newline, terminate them with
// an EOF token and then returns them. This function is used to
// create a new list of tokens for `#if` arguments.
static Token *copy_line(Token **rest, Token *tok) {
  Token head = {};
  Token *cur = &head;
  for (; !tok->at_bol; tok = tok->next) {
    cur = cur->next = copy_token(tok);
  }
  cur->next = new_eof(tok);
  *rest = tok;
  return head.next;
}

static Token *new_num_token(int val, Token *tmpl) {
  char *p, *buf;
  p = buf = malloc(13);
  p = FormatInt32(p, val);
  p[0] = '\n';
  p[1] = 0;
  return tokenize(new_file(tmpl->file->name, tmpl->file->file_no, buf));
}

static Token *read_const_expr(Token **rest, Token *tok) {
  tok = copy_line(rest, tok);
  Token head = {};
  Token *cur = &head;
  while (tok->kind != TK_EOF) {
    // "defined(foo)" or "defined foo" becomes "1" if macro "foo"
    // is defined. Otherwise "0".
    if (EQUAL(tok, "defined")) {
      Token *start = tok;
      bool has_paren = CONSUME(&tok, tok->next, "(");
      if (tok->kind != TK_IDENT)
        error_tok(start, "macro name must be an identifier");
      Macro *m = find_macro(tok);
      tok = tok->next;
      if (has_paren) tok = skip(tok, ')');
      cur = cur->next = new_num_token(m ? 1 : 0, start);
      continue;
    }
    cur = cur->next = tok;
    tok = tok->next;
  }
  cur->next = tok;
  return head.next;
}

// Read and evaluate a constant expression.
static long eval_const_expr(Token **rest, Token *tok) {
  Token *start = tok;
  Token *expr = read_const_expr(rest, tok->next);
  expr = preprocess2(expr);
  if (expr->kind == TK_EOF) error_tok(start, "no expression");
  // [https://www.sigbus.info/n1570#6.10.1p4] The standard requires
  // we replace remaining non-macro identifiers with "0" before
  // evaluating a constant expression. For example, `#if foo` is
  // equivalent to `#if 0` if foo is not defined.
  for (Token *t = expr; t->kind != TK_EOF; t = t->next) {
    if (t->kind == TK_IDENT) {
      Token *next = t->next;
      *t = *new_num_token(0, t);
      t->next = next;
    }
  }
  // Convert pp-numbers to regular numbers
  convert_pp_tokens(expr);
  Token *rest2;
  long val = const_expr(&rest2, expr);
  if (rest2->kind != TK_EOF && rest2->kind != TK_JAVADOWN) {
    error_tok(rest2, "extra token");
  }
  return val;
}

static CondIncl *push_cond_incl(Token *tok, bool included) {
  CondIncl *ci = calloc(1, sizeof(CondIncl));
  ci->next = cond_incl;
  ci->ctx = IN_THEN;
  ci->tok = tok;
  ci->included = included;
  cond_incl = ci;
  return ci;
}

static Macro *find_macro(Token *tok) {
  if (tok->kind != TK_IDENT) return NULL;
  return hashmap_get2(&macros, tok->loc, tok->len);
}

static Macro *add_macro(char *name, bool is_objlike, Token *body) {
  Macro *m = calloc(1, sizeof(Macro));
  m->name = name;
  m->is_objlike = is_objlike;
  m->body = body;
  hashmap_put(&macros, name, m);
  return m;
}

static MacroParam *read_macro_params(Token **rest, Token *tok,
                                     char **va_args_name) {
  MacroParam head = {};
  MacroParam *cur = &head;
  while (!EQUAL(tok, ")")) {
    if (cur != &head) tok = skip(tok, ',');
    if (EQUAL(tok, "...")) {
      *va_args_name = "__VA_ARGS__";
      *rest = skip(tok->next, ')');
      return head.next;
    }
    if (tok->kind == TK_JAVADOWN) {
      tok = tok->next;
    }
    if (tok->kind != TK_IDENT) {
      error_tok(tok, "expected an identifier");
    }
    if (EQUAL(tok->next, "...")) {
      *va_args_name = strndup(tok->loc, tok->len);
      *rest = skip(tok->next->next, ')');
      return head.next;
    }
    MacroParam *m = calloc(1, sizeof(MacroParam));
    m->name = strndup(tok->loc, tok->len);
    cur = cur->next = m;
    tok = tok->next;
  }
  *rest = tok->next;
  return head.next;
}

static Macro *read_macro_definition(Token **rest, Token *tok) {
  Macro *m;
  char *name;
  if (tok->kind != TK_IDENT) error_tok(tok, "macro name must be an identifier");
  name = strndup(tok->loc, tok->len);
  tok = tok->next;
  if (!tok->has_space && tok->len == 1 && tok->loc[0] == '(') {
    // Function-like macro
    char *va_args_name = NULL;
    MacroParam *params = read_macro_params(&tok, tok->next, &va_args_name);
    m = add_macro(name, false, copy_line(rest, tok));
    m->params = params;
    m->va_args_name = va_args_name;
  } else {
    // Object-like macro
    m = add_macro(name, true, copy_line(rest, tok));
  }
  return m;
}

static MacroArg *read_macro_arg_one(Token **rest, Token *tok, bool read_rest) {
  Token head = {};
  Token *cur = &head;
  int level = 0;
  for (;;) {
    if (level == 0 && tok->len == 1 && tok->loc[0] == ')') {
      break;
    }
    if (level == 0 && !read_rest && tok->len == 1 && tok->loc[0] == ',') {
      break;
    }
    if (tok->kind == TK_EOF) error_tok(tok, "premature end of input");
    if (tok->len == 1 && tok->loc[0] == '(') {
      level++;
    } else if (tok->len == 1 && tok->loc[0] == ')') {
      level--;
    }
    cur = cur->next = copy_token(tok);
    tok = tok->next;
  }
  cur->next = new_eof(tok);
  MacroArg *arg = calloc(1, sizeof(MacroArg));
  arg->tok = head.next;
  *rest = tok;
  return arg;
}

static MacroArg *read_macro_args(Token **rest, Token *tok, MacroParam *params,
                                 char *va_args_name) {
  Token *start = tok;
  tok = tok->next->next;
  MacroArg head = {};
  MacroArg *cur = &head;
  MacroParam *pp = params;
  for (; pp; pp = pp->next) {
    if (cur != &head) tok = skip(tok, ',');
    cur = cur->next = read_macro_arg_one(&tok, tok, false);
    cur->name = pp->name;
  }
  if (va_args_name) {
    MacroArg *arg;
    if (tok->len == 1 && tok->loc[0] == ')') {
      arg = calloc(1, sizeof(MacroArg));
      arg->tok = new_eof(tok);
    } else {
      if (pp != params) tok = skip(tok, ',');
      arg = read_macro_arg_one(&tok, tok, true);
    }
    arg->name = va_args_name;
    arg->is_va_args = true;
    cur = cur->next = arg;
  } else if (pp) {
    error_tok(start, "too many arguments");
  }
  skip(tok, ')');
  *rest = tok;
  return head.next;
}

static MacroArg *find_arg(MacroArg *args, Token *tok) {
  for (MacroArg *ap = args; ap; ap = ap->next) {
    if (tok->len == strlen(ap->name) &&
        !strncmp(tok->loc, ap->name, tok->len)) {
      return ap;
    }
  }
  return NULL;
}

// Concatenates all tokens in `tok` and returns a new string.
static char *join_tokens(Token *tok, Token *end) {
  // Compute the length of the resulting token.
  int len = 1;
  for (Token *t = tok; t != end && t->kind != TK_EOF; t = t->next) {
    if (t != tok && t->has_space) len++;
    len += t->len;
  }
  char *buf = calloc(1, len);
  // Copy token texts.
  int pos = 0;
  for (Token *t = tok; t != end && t->kind != TK_EOF; t = t->next) {
    if (t != tok && t->has_space) buf[pos++] = ' ';
    strncpy(buf + pos, t->loc, t->len);
    pos += t->len;
  }
  buf[pos] = '\0';
  return buf;
}

// Concatenates all tokens in `arg` and returns a new string token.
// This function is used for the stringizing operator (#).
static Token *stringize(Token *hash, Token *arg) {
  // Create a new string token. We need to set some value to its
  // source location for error reporting function, so we use a macro
  // name token as a template.
  char *s = join_tokens(arg, NULL);
  return new_str_token(s, hash);
}

// Concatenate two tokens to create a new token.
static Token *paste(Token *lhs, Token *rhs) {
  // Paste the two tokens.
  char *buf = xasprintf("%.*s%.*s", lhs->len, lhs->loc, rhs->len, rhs->loc);
  // Tokenize the resulting string.
  Token *tok = tokenize(new_file(lhs->file->name, lhs->file->file_no, buf));
  if (tok->next->kind != TK_EOF)
    error_tok(lhs, "pasting forms '%s', an invalid token", buf);
  return tok;
}

static bool has_varargs(MacroArg *args) {
  for (MacroArg *ap = args; ap; ap = ap->next) {
    if (!strcmp(ap->name, "__VA_ARGS__")) {
      return ap->tok->kind != TK_EOF;
    }
  }
  return false;
}

// Replace func-like macro parameters with given arguments.
static Token *subst(Token *tok, MacroArg *args) {
  Token head = {};
  Token *cur = &head;
  while (tok->kind != TK_EOF) {
    // "#" followed by a parameter is replaced with stringized actuals.
    if (tok->len == 1 && tok->loc[0] == '#') {
      MacroArg *arg = find_arg(args, tok->next);
      if (!arg) {
        error_tok(tok->next, "'#' is not followed by a macro parameter");
      }
      cur = cur->next = stringize(tok, arg->tok);
      tok = tok->next->next;
      continue;
    }
    // [GNU] If __VA_ARG__ is empty, `,##__VA_ARGS__` is expanded
    // to the empty token list. Otherwise, its expaned to `,` and
    // __VA_ARGS__.
    if (tok->len == 1 && tok->loc[0] == ',' &&
        (tok->next->len == 2 &&
         (tok->next->loc[0] == '#' && tok->next->loc[1] == '#'))) {
      MacroArg *arg = find_arg(args, tok->next->next);
      if (arg && arg->is_va_args) {
        if (arg->tok->kind == TK_EOF) {
          tok = tok->next->next->next;
        } else {
          cur = cur->next = copy_token(tok);
          tok = tok->next->next;
        }
        continue;
      }
    }
    if (tok->len == 2 && tok->loc[0] == '#' && tok->loc[1] == '#') {
      if (cur == &head)
        error_tok(tok, "'##' cannot appear at start of macro expansion");
      if (tok->next->kind == TK_EOF)
        error_tok(tok, "'##' cannot appear at end of macro expansion");
      MacroArg *arg = find_arg(args, tok->next);
      if (arg) {
        if (arg->tok->kind != TK_EOF) {
          *cur = *paste(cur, arg->tok);
          for (Token *t = arg->tok->next; t->kind != TK_EOF; t = t->next)
            cur = cur->next = copy_token(t);
        }
        tok = tok->next->next;
        continue;
      }
      *cur = *paste(cur, tok->next);
      tok = tok->next->next;
      continue;
    }
    MacroArg *arg = find_arg(args, tok);
    if (arg && (tok->next->len == 2 &&
                (tok->next->loc[0] == '#' && tok->next->loc[1] == '#'))) {
      Token *rhs = tok->next->next;
      if (arg->tok->kind == TK_EOF) {
        MacroArg *arg2 = find_arg(args, rhs);
        if (arg2) {
          for (Token *t = arg2->tok; t->kind != TK_EOF; t = t->next)
            cur = cur->next = copy_token(t);
        } else {
          cur = cur->next = copy_token(rhs);
        }
        tok = rhs->next;
        continue;
      }
      for (Token *t = arg->tok; t->kind != TK_EOF; t = t->next)
        cur = cur->next = copy_token(t);
      tok = tok->next;
      continue;
    }
    // If __VA_ARG__ is empty, __VA_OPT__(x) is expanded to the
    // empty token list. Otherwise, __VA_OPT__(x) is expanded to x.
    if (EQUAL(tok, "__VA_OPT__") && EQUAL(tok->next, "(")) {
      MacroArg *arg = read_macro_arg_one(&tok, tok->next->next, true);
      if (has_varargs(args))
        for (Token *t = arg->tok; t->kind != TK_EOF; t = t->next)
          cur = cur->next = t;
      tok = skip(tok, ')');
      continue;
    }
    // Handle a macro token. Macro arguments are completely macro-expanded
    // before they are substituted into a macro body.
    if (arg) {
      Token *t = preprocess2(arg->tok);
      t->at_bol = tok->at_bol;
      t->has_space = tok->has_space;
      for (; t->kind != TK_EOF; t = t->next) cur = cur->next = copy_token(t);
      tok = tok->next;
      continue;
    }
    // Handle a non-macro token.
    cur = cur->next = copy_token(tok);
    tok = tok->next;
    continue;
  }
  cur->next = tok;
  return head.next;
}

// If tok is a macro, expand it and return true.
// Otherwise, do nothing and return false.
static bool expand_macro(Token **rest, Token *tok) {
  if (hideset_contains(tok->hideset, tok->loc, tok->len)) return false;
  Macro *m = find_macro(tok);
  if (!m) return false;
  // Built-in dynamic macro application such as __LINE__
  if (m->handler) {
    *rest = m->handler(tok);
    (*rest)->next = tok->next;
    return true;
  }
  // Object-like macro application
  if (m->is_objlike) {
    Hideset *hs = hideset_union(tok->hideset, new_hideset(m->name));
    Token *body = add_hideset(m->body, hs);
    for (Token *t = body; t->kind != TK_EOF; t = t->next) {
      t->origin = tok;
    }
    *rest = append(body, tok->next);
    (*rest)->at_bol = tok->at_bol;
    (*rest)->has_space = tok->has_space;
    return true;
  }
  // If a funclike macro token is not followed by an argument list,
  // treat it as a normal identifier.
  if (!(tok->next->len == 1 && tok->next->loc[0] == '(')) return false;
  // Function-like macro application
  Token *macro_token = tok;
  MacroArg *args = read_macro_args(&tok, tok, m->params, m->va_args_name);
  Token *rparen = tok;
  // Tokens that consist a func-like macro invocation may have different
  // hidesets, and if that's the case, it's not clear what the hideset
  // for the new tokens should be. We take the interesection of the
  // macro token and the closing parenthesis and use it as a new hideset
  // as explained in the Dave Prossor's algorithm.
  Hideset *hs = hideset_intersection(macro_token->hideset, rparen->hideset);
  hs = hideset_union(hs, new_hideset(m->name));
  Token *body = subst(m->body, args);
  body = add_hideset(body, hs);
  for (Token *t = body; t->kind != TK_EOF; t = t->next) t->origin = macro_token;
  *rest = append(body, tok->next);
  (*rest)->at_bol = macro_token->at_bol;
  (*rest)->has_space = macro_token->has_space;
  return true;
}

char *search_include_paths(char *filename) {
  if (filename[0] == '/') return filename;
  static HashMap cache;
  char *cached = hashmap_get(&cache, filename);
  if (cached) return cached;
  // Search a file from the include paths.
  for (int i = 0; i < include_paths.len; i++) {
    char *path = xjoinpaths(include_paths.data[i], filename);
    if (!fileexists(path)) continue;
    hashmap_put(&cache, filename, path);
    include_next_idx = i + 1;
    return path;
  }
  return NULL;
}

static char *search_include_next(char *filename) {
  for (; include_next_idx < include_paths.len; include_next_idx++) {
    char *path =
        xasprintf("%s/%s", include_paths.data[include_next_idx], filename);
    if (fileexists(path)) return path;
  }
  return NULL;
}

// Read an #include argument.
static char *read_include_filename(Token **rest, Token *tok, bool *is_dquote) {
  // Pattern 1: #include "foo.h"
  if (tok->kind == TK_STR) {
    // A double-quoted filename for #include is a special kind of
    // token, and we don't want to interpret any escape sequences in it.
    // For example, "\f" in "C:\foo" is not a formfeed character but
    // just two non-control characters, backslash and f.
    // So we don't want to use token->str.
    *is_dquote = true;
    *rest = skip_line(tok->next);
    return strndup(tok->loc + 1, tok->len - 2);
  }
  // Pattern 2: #include <foo.h>
  if (EQUAL(tok, "<")) {
    // Reconstruct a filename from a sequence of tokens between
    // "<" and ">".
    Token *start = tok;
    // Find closing ">".
    for (; !EQUAL(tok, ">"); tok = tok->next)
      if (tok->at_bol || tok->kind == TK_EOF) error_tok(tok, "expected '>'");
    *is_dquote = false;
    *rest = skip_line(tok->next);
    return join_tokens(start->next, tok);
  }
  // Pattern 3: #include FOO
  // In this case FOO must be macro-expanded to either
  // a single string token or a sequence of "<" ... ">".
  if (tok->kind == TK_IDENT) {
    Token *tok2 = preprocess2(copy_line(rest, tok));
    return read_include_filename(&tok2, tok2, is_dquote);
  }
  error_tok(tok, "expected a filename");
}

// Detect the following "include guard" pattern.
//
//   #ifndef FOO_H
//   #define FOO_H
//   ...
//   #endif
static char *detect_include_guard(Token *tok) {
  // Detect the first two lines.
  if (!is_hash(tok) || !EQUAL(tok->next, "ifndef")) return NULL;
  tok = tok->next->next;
  if (tok->kind != TK_IDENT) return NULL;
  char *macro = strndup(tok->loc, tok->len);
  tok = tok->next;
  if (!is_hash(tok) || !EQUAL(tok->next, "define") ||
      !equal(tok->next->next, macro, strlen(macro)))
    return NULL;
  // Read until the end of the file.
  while (tok->kind != TK_EOF) {
    if (!is_hash(tok)) {
      tok = tok->next;
      continue;
    }
    if (EQUAL(tok->next, "endif") && tok->next->next->kind == TK_EOF)
      return macro;
    if (EQUAL(tok, "if") || EQUAL(tok, "ifdef") || EQUAL(tok, "ifndef"))
      tok = skip_cond_incl(tok->next);
    else
      tok = tok->next;
  }
  return NULL;
}

static Token *include_file(Token *tok, char *path, Token *filename_tok) {
  // Check for "#pragma once"
  if (hashmap_get(&pragma_once, path)) return tok;
  // If we read the same file before, and if the file was guarded
  // by the usual #ifndef ... #endif pattern, we may be able to
  // skip the file without opening it.
  static HashMap include_guards;
  char *guard_name = hashmap_get(&include_guards, path);
  if (guard_name && hashmap_get(&macros, guard_name)) return tok;
  Token *tok2 = tokenize_file(path);
  if (!tok2)
    error_tok(filename_tok, "%s: cannot open file: %s", path, strerror(errno));
  guard_name = detect_include_guard(tok2);
  if (guard_name) hashmap_put(&include_guards, path, guard_name);
  return append(tok2, tok);
}

// Read #line arguments
static void read_line_marker(Token **rest, Token *tok) {
  // TODO: This is broken if file is different? See gperf codegen.
  Token *start = tok;
  tok = preprocess(copy_line(rest, tok));
  if (tok->kind != TK_NUM || tok->ty->kind != TY_INT)
    error_tok(tok, "invalid line marker");
  start->file->line_delta = tok->val - start->line_no;
  tok = tok->next;
  if (tok->kind == TK_EOF) return;
  if (tok->kind != TK_STR) error_tok(tok, "filename expected");
  start->file->display_name = tok->str;
}

// Visit all tokens in `tok` while evaluating preprocessing
// macros and directives.
static Token *preprocess2(Token *tok) {
  unsigned char kw;
  Token head = {};
  Token *cur = &head;
  while (tok->kind != TK_EOF) {
    // If it is a macro, expand it.
    if (expand_macro(&tok, tok)) continue;
    // make sure javadown is removed if it's for a macro definition
    if (tok->kind == TK_JAVADOWN && is_hash(tok->next) &&
        EQUAL(tok->next->next, "define")) {
      read_macro_definition(&tok, tok->next->next->next)->javadown = tok;
      continue;
    }
    // Pass through if it is not a "#".
    if (!is_hash(tok)) {
      tok->line_delta = tok->file->line_delta;
      tok->filename = tok->file->display_name;
      cur = cur->next = tok;
      tok = tok->next;
      continue;
    }
    Token *start = tok;
    tok = tok->next;
    if ((kw = GetKw(tok->loc, tok->len))) {
      if (kw == KW_INCLUDE) {
        bool is_dquote;
        char *filename = read_include_filename(&tok, tok->next, &is_dquote);
        if (filename[0] != '/' && is_dquote) {
          char *tmp = strdup(start->file->name);
          char *path = xasprintf("%s/%s", dirname(tmp), filename);
          free(tmp);
          bool exists = fileexists(path);
          if (exists) {
            tok = include_file(tok, path, start->next->next);
            free(path);
            continue;
          }
          free(path);
        }
        char *path = search_include_paths(filename);
        tok = include_file(tok, path ? path : filename, start->next->next);
        continue;
      }
      if (kw == KW_INCLUDE_NEXT) {
        bool ignore;
        char *filename = read_include_filename(&tok, tok->next, &ignore);
        char *path = search_include_next(filename);
        tok = include_file(tok, path ? path : filename, start->next->next);
        continue;
      }
      if (kw == KW_DEFINE) {
        read_macro_definition(&tok, tok->next);
        continue;
      }
      if (kw == KW_UNDEF) {
        tok = tok->next;
        if (tok->kind != TK_IDENT)
          error_tok(tok, "macro name must be an identifier");
        undef_macro(strndup(tok->loc, tok->len));
        tok = skip_line(tok->next);
        continue;
      }
      if (kw == KW_IF) {
        long val = eval_const_expr(&tok, tok);
        push_cond_incl(start, val);
        if (!val) tok = skip_cond_incl(tok);
        continue;
      }
      if (kw == KW_IFDEF) {
        bool defined = find_macro(tok->next);
        push_cond_incl(tok, defined);
        tok = skip_line(tok->next->next);
        if (!defined) tok = skip_cond_incl(tok);
        continue;
      }
      if (kw == KW_IFNDEF) {
        bool defined = find_macro(tok->next);
        push_cond_incl(tok, !defined);
        tok = skip_line(tok->next->next);
        if (defined) tok = skip_cond_incl(tok);
        continue;
      }
      if (kw == KW_ELIF) {
        if (!cond_incl || cond_incl->ctx == IN_ELSE)
          error_tok(start, "stray #elif");
        cond_incl->ctx = IN_ELIF;
        if (!cond_incl->included && eval_const_expr(&tok, tok))
          cond_incl->included = true;
        else
          tok = skip_cond_incl(tok);
        continue;
      }
      if (kw == KW_ELSE) {
        if (!cond_incl || cond_incl->ctx == IN_ELSE)
          error_tok(start, "stray #else");
        cond_incl->ctx = IN_ELSE;
        tok = skip_line(tok->next);
        if (cond_incl->included) tok = skip_cond_incl(tok);
        continue;
      }
      if (kw == KW_ENDIF) {
        if (!cond_incl) error_tok(start, "stray #endif");
        cond_incl = cond_incl->next;
        tok = skip_line(tok->next);
        continue;
      }
      if (kw == KW_LINE) {
        read_line_marker(&tok, tok->next);
        continue;
      }
    }
    if (tok->kind == TK_PP_NUM) {
      read_line_marker(&tok, tok);
      continue;
    }
    if (kw == KW_PRAGMA && EQUAL(tok->next, "once")) {
      hashmap_put(&pragma_once, tok->file->name, (void *)1);
      tok = skip_line(tok->next->next);
      continue;
    }
    if (kw == KW_PRAGMA) {
      do {
        tok = tok->next;
      } while (!tok->at_bol);
      continue;
    }
    if (kw == KW_ERROR) {
      error_tok(tok, "error");
    }
    // `#`-only line is legal. It's called a null directive.
    if (tok->at_bol) continue;
    error_tok(tok, "invalid preprocessor directive");
  }
  cur->next = tok;
  return head.next;
}

void define_macro(char *name, char *buf) {
  Token *tok = tokenize(new_file("<built-in>", 1, buf));
  add_macro(name, true, tok);
}

void undef_macro(char *name) {
  hashmap_delete(&macros, name);
}

static Macro *add_builtin(char *name, macro_handler_fn *fn) {
  Macro *m = add_macro(name, true, NULL);
  m->handler = fn;
  return m;
}

static Token *file_macro(Token *tmpl) {
  while (tmpl->origin) tmpl = tmpl->origin;
  return new_str_token(tmpl->file->display_name, tmpl);
}

static Token *line_macro(Token *tmpl) {
  while (tmpl->origin) tmpl = tmpl->origin;
  int i = tmpl->line_no + tmpl->file->line_delta;
  return new_num_token(i, tmpl);
}

// __COUNTER__ is expanded to serial values starting from 0.
static Token *counter_macro(Token *tmpl) {
  static int i = 0;
  return new_num_token(i++, tmpl);
}

// __TIMESTAMP__ is expanded to a string describing the last
// modification time of the current file. E.g.
// "Fri Jul 24 01:32:50 2020"
static Token *timestamp_macro(Token *tmpl) {
  struct stat st;
  if (stat(tmpl->file->name, &st) != 0)
    return new_str_token("??? ??? ?? ??:??:?? ????", tmpl);
  char buf[64];
  ctime_r(&st.st_mtim.tv_sec, buf);
  buf[24] = '\0';
  return new_str_token(buf, tmpl);
}

static Token *base_file_macro(Token *tmpl) {
  return new_str_token(base_file, tmpl);
}

// __DATE__ is expanded to the current date, e.g. "May 17 2020".
static char *format_date(struct tm *tm) {
  return xasprintf("\"%s %2d %d\"", kMonthNameShort[tm->tm_mon], tm->tm_mday,
                   tm->tm_year + 1900);
}

// __TIME__ is expanded to the current time, e.g. "13:34:03".
static char *format_time(struct tm *tm) {
  return xasprintf("\"%02d:%02d:%02d\"", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void init_macros_conditional(void) {
  if (opt_pg) define_macro("__PG__", "1");
  if (opt_pic) define_macro("__PIC__", "1");
  if (opt_sse3) define_macro("__SSE3__", "1");
  if (opt_sse4) define_macro("__SSE4__", "1");
  if (opt_popcnt) define_macro("__POPCNT__", "1");
  if (opt_fentry) define_macro("__MFENTRY__", "1");
}

void init_macros(void) {
  char *val, *name = "\
__chibicc__\000\
1\000\
__cosmopolitan__\000\
1\000\
__GNUC__\000\
9\000\
__GNUC_MINOR__\000\
0\000\
__GNUC_PATCHLEVEL__\000\
0\000\
__NO_INLINE__\000\
16\000\
__GNUC_STDC_INLINE__\000\
1\000\
__BIGGEST_ALIGNMENT__\000\
16\000\
__C99_MACRO_WITH_VA_ARGS\000\
1\000\
__GCC_ASM_FLAG_OUTPUTS__\000\
1\000\
__ELF__\000\
1\000\
__LP64__\000\
1\000\
_LP64\000\
1\000\
__STDC__\000\
1\000\
__STDC_HOSTED__\000\
1\000\
__STDC_NO_COMPLEX__\000\
1\000\
__STDC_UTF_16__\000\
1\000\
__STDC_UTF_32__\000\
1\000\
__STDC_VERSION__\000\
201112L\000\
__USER_LABEL_PREFIX__\000\
\000\
__alignof__\000\
_Alignof\000\
__const__\000\
const\000\
__inline__\000\
inline\000\
__signed__\000\
signed\000\
__typeof__\000\
typeof\000\
__volatile__\000\
volatile\000\
__unix\000\
1\000\
__unix__\000\
1\000\
__gnu_linux__\000\
1\000\
__BYTE_ORDER__\000\
1234\000\
__FLOAT_WORD_ORDER__\000\
1234\000\
__ORDER_BIG_ENDIAN__\000\
4321\000\
__ORDER_LITTLE_ENDIAN__\000\
1234\000\
__INT8_MAX__\000\
0x7f\000\
__UINT8_MAX__\000\
0xff\000\
__INT16_MAX__\000\
0x7fff\000\
__UINT16_MAX__\000\
0xffff\000\
__SHRT_MAX__\000\
0x7fff\000\
__INT_MAX__\000\
0x7fffffff\000\
__INT32_MAX__\000\
0x7fffffff\000\
__UINT32_MAX__\000\
0xffffffffu\000\
__INT64_MAX__\000\
0x7fffffffffffffffl\000\
__INTMAX_MAX__\000\
0x7fffffffffffffffl\000\
__LONG_MAX__\000\
0x7fffffffffffffffl\000\
__LONG_LONG_MAX__\000\
0x7fffffffffffffffl\000\
__UINT64_MAX__\000\
0xfffffffffffffffful\000\
__UINTMAX_MAX__\000\
0xfffffffffffffffful\000\
__SIZE_MAX__\000\
0xfffffffffffffffful\000\
__INTPTR_MAX__\000\
0x7fffffffffffffffl\000\
__UINTPTR_MAX__\000\
0xfffffffffffffffful\000\
__WINT_MAX__\000\
0xffffffffu\000\
__CHAR_BIT__\000\
8\000\
__SIZEOF_SHORT__\000\
2\000\
__SIZEOF_INT__\000\
4\000\
__SIZEOF_LONG__\000\
8\000\
__SIZEOF_LONG_LONG__\000\
8\000\
__SIZEOF_POINTER__\000\
8\000\
__SIZEOF_PTRDIFF_T__\000\
8\000\
__SIZEOF_SIZE_T__\000\
8\000\
__SIZEOF_WCHAR_T__\000\
4\000\
__SIZEOF_WINT_T__\000\
4\000\
__SIZEOF_FLOAT__\000\
4\000\
__SIZEOF_FLOAT128__\000\
16\000\
__SIZEOF_DOUBLE__\000\
8\000\
__SIZEOF_FLOAT80__\000\
16\000\
__SIZEOF_LONG_DOUBLE__\000\
16\000\
__INT8_TYPE__\000\
signed char\000\
__UINT8_TYPE__\000\
unsigned char\000\
__INT16_TYPE__\000\
short int\000\
__UINT16_TYPE__\000\
short unsigned int\000\
__INT32_TYPE__\000\
int\000\
__UINT32_TYPE__\000\
unsigned int\000\
__INT64_TYPE__\000\
long int\000\
__INTMAX_TYPE__\000\
long int\000\
__UINT64_TYPE__\000\
long unsigned int\000\
__UINTMAX_TYPE__\000\
long unsigned int\000\
__INTMAX_TYPE__\000\
long int\000\
__UINTMAX_TYPE__\000\
long unsigned int\000\
__INTPTR_TYPE__\000\
long int\000\
__UINTPTR_TYPE__\000\
long unsigned int\000\
__PTRDIFF_TYPE__\000\
long int\000\
__SIZE_TYPE__\000\
long unsigned int\000\
__WCHAR_TYPE__\000\
int\000\
__CHAR16_TYPE__\000\
short unsigned int\000\
__CHAR32_TYPE__\000\
unsigned int\000\
__WINT_TYPE__\000\
unsigned int\000\
__CHAR16_TYPE__\000\
short unsigned int\000\
__WCHAR_TYPE__\000\
int\000\
__CHAR32_TYPE__\000\
unsigned int\000\
__INT_LEAST8_TYPE__\000\
signed char\000\
__UINT_LEAST8_TYPE__\000\
unsigned char\000\
__INT_LEAST16_TYPE__\000\
int\000\
__UINT_LEAST16_TYPE__\000\
unsigned short\000\
__INT_LEAST32_TYPE__\000\
short\000\
__UINT_LEAST32_TYPE__\000\
unsigned int\000\
__INT_LEAST64_TYPE__\000\
long\000\
__UINT_LEAST64_TYPE__\000\
unsigned long\000\
__INT_FAST8_TYPE__\000\
signed char\000\
__UINT_FAST8_TYPE__\000\
unsigned char\000\
__INT_FAST16_TYPE__\000\
int\000\
__UINT_FAST16_TYPE__\000\
unsigned\000\
__INT_FAST32_TYPE__\000\
int\000\
__UINT_FAST32_TYPE__\000\
unsigned\000\
__INT_FAST8_MAX__\000\
0x7f\000\
__INT_FAST16_MAX__\000\
0x7fffffff\000\
__INT_FAST32_MAX__\000\
0x7fffffff\000\
__INT_FAST64_MAX__\000\
0x7fffffffffffffffl\000\
__INT_FAST64_TYPE__\000\
long\000\
__UINT_FAST64_TYPE__\000\
unsigned long\000\
__DBL_DECIMAL_DIG__\000\
17\000\
__DBL_DENORM_MIN__\000\
((double)4.94065645841246544176568792868221372e-324L)\000\
__DBL_DIG__\000\
15\000\
__DBL_EPSILON__\000\
((double)2.22044604925031308084726333618164062e-16L)\000\
__DBL_HAS_DENORM__\000\
1\000\
__DBL_HAS_INFINITY__\000\
1\000\
__DBL_HAS_QUIET_NAN__\000\
1\000\
__DBL_MANT_DIG__\000\
53\000\
__DBL_MAX_10_EXP__\000\
308\000\
__DBL_MAX_EXP__\000\
1024\000\
__DBL_MAX__\000\
((double)1.79769313486231570814527423731704357e+308L)\000\
__DBL_MIN_10_EXP__\000\
(-307)\000\
__DBL_MIN_EXP__\000\
(-1021)\000\
__DBL_MIN__\000\
((double)2.22507385850720138309023271733240406e-308L)\000\
__FLT_DECIMAL_DIG__\000\
9\000\
__FLT_DENORM_MIN__\000\
1.40129846432481707092372958328991613e-45F\000\
__FLT_DIG__\000\
6\000\
__FLT_EPSILON__\000\
1.19209289550781250000000000000000000e-7F\000\
__FLT_EVAL_METHOD_TS_18661_3__\000\
0\000\
__FLT_EVAL_METHOD__\000\
0\000\
__FLT_HAS_DENORM__\000\
1\000\
__FLT_HAS_INFINITY__\000\
1\000\
__FLT_HAS_QUIET_NAN__\000\
1\000\
__FLT_MANT_DIG__\000\
24\000\
__FLT_MAX_10_EXP__\000\
38\000\
__FLT_MAX_EXP__\000\
128\000\
__FLT_MAX__\000\
3.40282346638528859811704183484516925e+38F\000\
__FLT_MIN_10_EXP__\000\
(-37)\000\
__FLT_MIN_EXP__\000\
(-125)\000\
__FLT_MIN__\000\
1.17549435082228750796873653722224568e-38F\000\
__FLT_RADIX__\000\
2\000\
__LDBL_DECIMAL_DIG__\000\
21\000\
__LDBL_DENORM_MIN__\000\
3.64519953188247460252840593361941982e-4951L\000\
__LDBL_DIG__\000\
18\000\
__LDBL_EPSILON__\000\
1.08420217248550443400745280086994171e-19L\000\
__LDBL_HAS_DENORM__\000\
1\000\
__LDBL_HAS_INFINITY__\000\
1\000\
__LDBL_HAS_QUIET_NAN__\000\
1\000\
__LDBL_MANT_DIG__\000\
64\000\
__LDBL_MAX_10_EXP__\000\
4932\000\
__LDBL_MAX_EXP__\000\
16384\000\
__LDBL_MAX__\000\
1.18973149535723176502126385303097021e+4932L\000\
__LDBL_MIN_10_EXP__\000\
(-4931)\000\
__LDBL_MIN_EXP__\000\
(-16381)\000\
__LDBL_MIN__\000\
3.36210314311209350626267781732175260e-4932L\000\
__x86_64\000\
1\000\
__x86_64__\000\
1\000\
__amd64\000\
1\000\
__amd64__\000\
1\000\
__MMX__\000\
1\000\
__SSE__\000\
1\000\
__SSE_MATH__\000\
1\000\
__SSE2__\000\
1\000\
__SSE2_MATH__\000\
1\000\
__ATOMIC_ACQUIRE\000\
2\000\
__ATOMIC_HLE_RELEASE\000\
131072\000\
__ATOMIC_HLE_ACQUIRE\000\
65536\000\
__ATOMIC_RELAXED\000\
0\000\
__ATOMIC_CONSUME\000\
1\000\
__ATOMIC_SEQ_CST\000\
5\000\
__ATOMIC_ACQ_REL\000\
4\000\
__ATOMIC_RELEASE\000\
3\000\
\000";
  do {
    val = name + strlen(name) + 1;
    define_macro(name, val);
    name = val + strlen(val) + 1;
  } while (*name);
  add_builtin("__FILE__", file_macro);
  add_builtin("__LINE__", line_macro);
  add_builtin("__COUNTER__", counter_macro);
  add_builtin("__TIMESTAMP__", timestamp_macro);
  add_builtin("__BASE_FILE__", base_file_macro);
  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  define_macro("__DATE__", format_date(tm));
  define_macro("__TIME__", format_time(tm));
}

static StringKind getStringKind(Token *tok) {
  if (!strcmp(tok->loc, "u8")) return STR_UTF8;
  switch (tok->loc[0]) {
    case '"':
      return STR_NONE;
    case 'u':
      return STR_UTF16;
    case 'U':
      return STR_UTF32;
    case 'L':
      return STR_WIDE;
  }
  UNREACHABLE();
}

// Concatenate adjacent string literals into a single string literal
// as per the C spec.
static void join_adjacent_string_literals(Token *tok) {
  // First pass: If regular string literals are adjacent to wide
  // string literals, regular string literals are converted to a wide
  // type before concatenation. In this pass, we do the conversion.
  for (Token *tok1 = tok; tok1->kind != TK_EOF;) {
    if (tok1->kind != TK_STR || tok1->next->kind != TK_STR) {
      tok1 = tok1->next;
      continue;
    }
    StringKind kind = getStringKind(tok1);
    Type *basety = tok1->ty->base;
    for (Token *t = tok1->next; t->kind == TK_STR; t = t->next) {
      StringKind k = getStringKind(t);
      if (kind == STR_NONE) {
        kind = k;
        basety = t->ty->base;
      } else if (k != STR_NONE && kind != k) {
        error_tok(t,
                  "unsupported non-standard concatenation of string literals");
      }
    }
    if (basety->size > 1) {
      for (Token *t = tok1; t->kind == TK_STR; t = t->next) {
        if (t->ty->base->size == 1) {
          *t = *tokenize_string_literal(t, basety);
        }
      }
    }
    while (tok1->kind == TK_STR) tok1 = tok1->next;
  }
  // Second pass: concatenate adjacent string literals.
  for (Token *tok1 = tok; tok1->kind != TK_EOF;) {
    if (tok1->kind != TK_STR || tok1->next->kind != TK_STR) {
      tok1 = tok1->next;
      continue;
    }
    Token *tok2 = tok1->next;
    while (tok2->kind == TK_STR) tok2 = tok2->next;
    int len = tok1->ty->array_len;
    for (Token *t = tok1->next; t != tok2; t = t->next) {
      len = len + t->ty->array_len - 1;
    }
    char *buf = calloc(tok1->ty->base->size, len);
    int j = 0;
    int i = 0;
    for (Token *t = tok1; t != tok2; t = t->next) {
      ++j;
      memcpy(buf + i, t->str, t->ty->size);
      i = i + t->ty->size - t->ty->base->size;
    }
    *tok1 = *copy_token(tok1);
    tok1->ty = array_of(tok1->ty->base, len);
    tok1->str = buf;
    tok1->next = tok2;
    tok1 = tok2;
  }
}

// Entry point function of the preprocessor.
Token *preprocess(Token *tok) {
  tok = preprocess2(tok);
  if (cond_incl) {
    error_tok(cond_incl->tok, "unterminated conditional directive");
  }
  convert_pp_tokens(tok);
  join_adjacent_string_literals(tok);
  for (Token *t = tok; t; t = t->next) {
    t->line_no += t->line_delta;
  }
  return tok;
}
