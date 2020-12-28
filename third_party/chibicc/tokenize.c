#include "third_party/chibicc/chibicc.h"
#include "third_party/chibicc/file.h"

#define LOOKINGAT(TOK, OP) (!memcmp(TOK, OP, sizeof(OP) - 1))

// Input file
static File *current_file;

// A list of all input files.
static File **input_files;

// True if the current position is at the beginning of a line
static bool at_bol;

// True if the current position follows a space character
static bool has_space;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error message in the following format.
//
// foo.c:10: x = y + 1;
//               ^ <error message here>
static void verror_at(char *filename, char *input, int line_no, char *loc,
                      char *fmt, va_list ap) {
  // Find a line containing `loc`.
  char *line = loc;
  while (input < line && line[-1] != '\n') line--;
  char *end = loc;
  while (*end && *end != '\n') end++;
  // Print out the line.
  int indent = fprintf(stderr, "%s:%d: ", filename, line_no);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);
  // Show the error message.
  int pos = display_width(line, loc - line) + indent;
  fprintf(stderr, "%*s", pos, "");  // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

void error_at(char *loc, char *fmt, ...) {
  int line_no = 1;
  for (char *p = current_file->contents; p < loc; p++) {
    if (*p == '\n') line_no++;
  }
  va_list ap;
  va_start(ap, fmt);
  verror_at(current_file->name, current_file->contents, line_no, loc, fmt, ap);
  va_end(ap);
  exit(1);
}

void error_tok(Token *tok, char *fmt, ...) {
  va_list ap, va;
  va_start(va, fmt);
  for (Token *t = tok; t; t = t->origin) {
    va_copy(ap, va);
    verror_at(t->file->name, t->file->contents, t->line_no, t->loc, fmt, ap);
    va_end(ap);
  }
  va_end(ap);
  exit(1);
}

void warn_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->file->name, tok->file->contents, tok->line_no, tok->loc, fmt,
            ap);
  va_end(ap);
}

static int is_space(int c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' ||
         c == '\v';
}

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op, size_t n) {
  return n == tok->len && !memcmp(tok->loc, op, tok->len);
}

bool consume(Token **rest, Token *tok, char *str, size_t n) {
  if (n == tok->len && !memcmp(tok->loc, str, n)) {
    *rest = tok->next;
    return true;
  }
  *rest = tok;
  return false;
}

// Ensure that the current token is `op`.
Token *skip(Token *tok, char op) {
  if (tok->len == 1 && *tok->loc == op) {
    return tok->next;
  } else {
    error_tok(tok, "expected '%c'", op);
  }
}

// Create a new token.
static Token *new_token(TokenKind kind, char *start, char *end) {
  Token *tok = alloc_token();
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start;
  tok->file = current_file;
  tok->filename = current_file->display_name;
  tok->at_bol = at_bol;
  tok->has_space = has_space;
  at_bol = has_space = false;
  return tok;
}

// Read an identifier and returns the length of it.
// If p does not point to a valid identifier, 0 is returned.
static int read_ident(char *start) {
  char *p = start;
  uint32_t c = decode_utf8(&p, p);
  if (!is_ident1(c)) return 0;
  for (;;) {
    char *q;
    c = decode_utf8(&q, p);
    if (!is_ident2(c)) {
      return p - start;
    }
    p = q;
  }
}

static int from_hex(char c) {
  if ('0' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'f') return c - 'a' + 10;
  return c - 'A' + 10;
}

// Read a punctuator token from p and returns its length.
int read_punct(char *p) {
  static char kw[][4] = {"<<=", ">>=", "...", "==", "!=", "<=", ">=", "->",
                         "+=",  "-=",  "*=",  "/=", "++", "--", "%=", "&=",
                         "|=",  "^=",  "&&",  "||", "<<", ">>", "##"};
  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
    for (int j = 0;;) {
      if (p[j] != kw[i][j]) break;
      if (!kw[i][++j]) return j;
    }
  }
  return ispunct(*p) ? 1 : 0;
}

static bool is_keyword(Token *tok) {
  static HashMap map;
  if (map.capacity == 0) {
    static char *kw[] = {
        "return",    "if",         "else",
        "for",       "while",      "int",
        "sizeof",    "char",       "struct",
        "union",     "short",      "long",
        "void",      "typedef",    "_Bool",
        "enum",      "static",     "goto",
        "break",     "continue",   "switch",
        "case",      "default",    "extern",
        "_Alignof",  "_Alignas",   "do",
        "signed",    "unsigned",   "const",
        "volatile",  "auto",       "register",
        "restrict",  "__restrict", "__restrict__",
        "_Noreturn", "float",      "double",
        "typeof",    "asm",        "_Thread_local",
        "__thread",  "_Atomic",    "__attribute__",
    };
    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
      hashmap_put(&map, kw[i], (void *)1);
    }
  }
  return hashmap_get2(&map, tok->loc, tok->len);
}

int read_escaped_char(char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    unsigned c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7') c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;
    return c;
  }
  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p)) error_at(p, "invalid hex escape sequence");
    unsigned c = 0;
    for (; isxdigit(*p); p++) {
      c = (c << 4) + from_hex(*p); /* TODO(jart): overflow here unicode_test */
    }
    *new_pos = p;
    return c;
  }
  *new_pos = p + 1;
  // Escape sequences are defined using themselves here. E.g.
  // '\n' is implemented using '\n'. This tautological definition
  // works because the compiler that compiles our compiler knows
  // what '\n' actually is. In other words, we "inherit" the ASCII
  // code of '\n' from the compiler that compiles our compiler,
  // so we don't have to teach the actual code here.
  //
  // This fact has huge implications not only for the correctness
  // of the compiler but also for the security of the generated code.
  // For more info, read "Reflections on Trusting Trust" by Ken Thompson.
  // https://github.com/rui314/chibicc/wiki/thompson1984.pdf
  switch (*p) {
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
    // [GNU] \e for the ASCII escape character is a GNU C extension.
    case 'e':
      return 27;
    default:
      return *p;
  }
}

// Find a closing double-quote.
static char *string_literal_end(char *p) {
  char *start = p;
  for (; *p != '"'; p++) {
    if (*p == '\n' || *p == '\0') error_at(start, "unclosed string literal");
    if (*p == '\\') p++;
  }
  return p;
}

static Token *read_string_literal(char *start, char *quote) {
  char *end = string_literal_end(quote + 1);
  char *buf = calloc(2, end - quote);
  int len = 0;
  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(&p, p + 1);
    else
      buf[len++] = *p++;
  }
  Token *tok = new_token(TK_STR, start, end + 1);
  tok->ty = array_of(ty_char, len + 1);
  tok->str = buf;
  return tok;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-16.
//
// UTF-16 is yet another variable-width encoding for Unicode. Code
// points smaller than U+10000 are encoded in 2 bytes. Code points
// equal to or larger than that are encoded in 4 bytes. Each 2 bytes
// in the 4 byte sequence is called "surrogate", and a 4 byte sequence
// is called a "surrogate pair".
static Token *read_utf16_string_literal(char *start, char *quote) {
  char *end = string_literal_end(quote + 1);
  uint16_t *buf = calloc(2, end - start - 1);
  int len = 0;
  for (char *p = quote + 1; p < end;) {
    if (*p == '\\') {
      buf[len++] = read_escaped_char(&p, p + 1);
      continue;
    }
    uint32_t c = decode_utf8(&p, p);
    if (c < 0x10000) {
      // Encode a code point in 2 bytes.
      buf[len++] = c;
    } else {
      // Encode a code point in 4 bytes.
      c -= 0x10000;
      buf[len++] = 0xd800 + ((c >> 10) & 0x3ff);
      buf[len++] = 0xdc00 + (c & 0x3ff);
    }
  }
  Token *tok = new_token(TK_STR, start, end + 1);
  tok->ty = array_of(ty_ushort, len + 1);
  tok->str = (char *)buf;
  return tok;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-32.
//
// UTF-32 is a fixed-width encoding for Unicode. Each code point is
// encoded in 4 bytes.
static Token *read_utf32_string_literal(char *start, char *quote, Type *ty) {
  char *end = string_literal_end(quote + 1);
  uint32_t *buf = calloc(4, end - quote);
  int len = 0;
  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(&p, p + 1);
    else
      buf[len++] = decode_utf8(&p, p);
  }
  Token *tok = new_token(TK_STR, start, end + 1);
  tok->ty = array_of(ty, len + 1);
  tok->str = (char *)buf;
  return tok;
}

static Token *read_char_literal(char *start, char *quote, Type *ty) {
  char *p = quote + 1;
  if (*p == '\0') error_at(start, "unclosed char literal");
  int c;
  if (*p == '\\')
    c = read_escaped_char(&p, p + 1);
  else
    c = decode_utf8(&p, p);
  char *end = strchr(p, '\'');
  if (!end) error_at(p, "unclosed char literal");
  Token *tok = new_token(TK_NUM, start, end + 1);
  tok->val = c;
  tok->ty = ty;
  return tok;
}

static bool convert_pp_int(Token *tok) {
  char *p = tok->loc;
  // Read a binary, octal, decimal or hexadecimal number.
  int base = 10;
  if (!strncasecmp(p, "0x", 2) && isxdigit(p[2])) {
    p += 2;
    base = 16;
  } else if (!strncasecmp(p, "0b", 2) && (p[2] == '0' || p[2] == '1')) {
    p += 2;
    base = 2;
  } else if (*p == '0') {
    base = 8;
  }
  int64_t val = strtoul(p, &p, base);
  // Read U, L or LL suffixes.
  bool l = false;
  bool u = false;
  if (LOOKINGAT(p, "LLU") || LOOKINGAT(p, "LLu") || LOOKINGAT(p, "llU") ||
      LOOKINGAT(p, "llu") || LOOKINGAT(p, "ULL") || LOOKINGAT(p, "Ull") ||
      LOOKINGAT(p, "uLL") || LOOKINGAT(p, "ull")) {
    p += 3;
    l = u = true;
  } else if (!strncasecmp(p, "lu", 2) || !strncasecmp(p, "ul", 2)) {
    p += 2;
    l = u = true;
  } else if (LOOKINGAT(p, "LL") || LOOKINGAT(p, "ll")) {
    p += 2;
    l = true;
  } else if (*p == 'L' || *p == 'l') {
    p++;
    l = true;
  } else if (*p == 'U' || *p == 'u') {
    p++;
    u = true;
  }
  if (p != tok->loc + tok->len) return false;
  // Infer a type.
  Type *ty;
  if (base == 10) {
    if (l && u)
      ty = ty_ulong;
    else if (l)
      ty = ty_long;
    else if (u)
      ty = (val >> 32) ? ty_ulong : ty_uint;
    else
      ty = (val >> 31) ? ty_long : ty_int;
  } else {
    if (l && u)
      ty = ty_ulong;
    else if (l)
      ty = (val >> 63) ? ty_ulong : ty_long;
    else if (u)
      ty = (val >> 32) ? ty_ulong : ty_uint;
    else if (val >> 63)
      ty = ty_ulong;
    else if (val >> 32)
      ty = ty_long;
    else if (val >> 31)
      ty = ty_uint;
    else
      ty = ty_int;
  }
  tok->kind = TK_NUM;
  tok->val = val;
  tok->ty = ty;
  return true;
}

// The definition of the numeric literal at the preprocessing stage
// is more relaxed than the definition of that at the later stages.
// In order to handle that, a numeric literal is tokenized as a
// "pp-number" token first and then converted to a regular number
// token after preprocessing.
//
// This function converts a pp-number token to a regular number token.
static void convert_pp_number(Token *tok) {
  // Try to parse as an integer constant.
  if (convert_pp_int(tok)) return;
  // If it's not an integer, it must be a floating point constant.
  char *end;
  long double val = strtold(tok->loc, &end);
  Type *ty;
  if (*end == 'f' || *end == 'F') {
    ty = ty_float;
    end++;
  } else if (*end == 'l' || *end == 'L') {
    ty = ty_ldouble;
    end++;
  } else {
    ty = ty_double;
  }
  if (tok->loc + tok->len != end) error_tok(tok, "invalid numeric constant");
  tok->kind = TK_NUM;
  tok->fval = val;
  tok->ty = ty;
}

void convert_pp_tokens(Token *tok) {
  for (Token *t = tok; t->kind != TK_EOF; t = t->next) {
    if (is_keyword(t))
      t->kind = TK_KEYWORD;
    else if (t->kind == TK_PP_NUM)
      convert_pp_number(t);
  }
}

// Initialize line info for all tokens.
static void add_line_numbers(Token *tok) {
  char *p = current_file->contents;
  int n = 1;
  do {
    if (p == tok->loc) {
      tok->line_no = n;
      tok = tok->next;
    }
    if (*p == '\n') n++;
  } while (*p++);
}

Token *tokenize_string_literal(Token *tok, Type *basety) {
  Token *t;
  if (basety->size == 2)
    t = read_utf16_string_literal(tok->loc, tok->loc);
  else
    t = read_utf32_string_literal(tok->loc, tok->loc, basety);
  t->next = tok->next;
  return t;
}

// Tokenize a given string and returns new tokens.
Token *tokenize(File *file) {
  current_file = file;
  char *p = file->contents;
  Token head = {};
  Token *cur = &head;
  struct Javadown *javadown;
  at_bol = true;
  has_space = false;
  while (*p) {
    // Skip line comments.
    if (LOOKINGAT(p, "//")) {
      p += 2;
      while (*p != '\n') p++;
      has_space = true;
      continue;
    }
    // Javadoc-style markdown comments.
    if (LOOKINGAT(p, "/**") && p[3] != '/' && p[3] != '*') {
      char *q = strstr(p + 3, "*/");
      if (!q) error_at(p, "unclosed javadown");
      javadown = ParseJavadown(p + 3, q - p - 3 - 2);
      if (javadown->isfileoverview) {
        FreeJavadown(file->javadown);
        file->javadown = javadown;
      } else {
        cur = cur->next = new_token(TK_JAVADOWN, p, q + 2);
        cur->javadown = javadown;
      }
      p = q + 2;
      has_space = true;
      continue;
    }
    // Skip block comments.
    if (LOOKINGAT(p, "/*")) {
      char *q = strstr(p + 2, "*/");
      if (!q) error_at(p, "unclosed block comment");
      p = q + 2;
      has_space = true;
      continue;
    }
    // Skip newline.
    if (*p == '\n') {
      p++;
      at_bol = true;
      has_space = false;
      continue;
    }
    // Skip whitespace characters.
    if (is_space(*p)) {
      p++;
      has_space = true;
      continue;
    }
    // Numeric literal
    if (isdigit(*p) || (*p == '.' && isdigit(p[1]))) {
      char *q = p++;
      for (;;) {
        if (p[0] && p[1] && strchr("eEpP", p[0]) && strchr("+-", p[1])) {
          p += 2;
        } else if (isalnum(*p) || *p == '.') {
          p++;
        } else {
          break;
        }
      }
      cur = cur->next = new_token(TK_PP_NUM, q, p);
      continue;
    }
    // String literal
    if (*p == '"') {
      cur = cur->next = read_string_literal(p, p);
      p += cur->len;
      continue;
    }
    // UTF-8 string literal
    if (LOOKINGAT(p, "u8\"")) {
      cur = cur->next = read_string_literal(p, p + 2);
      p += cur->len;
      continue;
    }
    // UTF-16 string literal
    if (LOOKINGAT(p, "u\"")) {
      cur = cur->next = read_utf16_string_literal(p, p + 1);
      p += cur->len;
      continue;
    }
    // Wide string literal
    if (LOOKINGAT(p, "L\"")) {
      cur = cur->next = read_utf32_string_literal(p, p + 1, ty_int);
      p += cur->len;
      continue;
    }
    // UTF-32 string literal
    if (LOOKINGAT(p, "U\"")) {
      cur = cur->next = read_utf32_string_literal(p, p + 1, ty_uint);
      p += cur->len;
      continue;
    }
    // Character literal
    if (*p == '\'') {
      cur = cur->next = read_char_literal(p, p, ty_int);
      cur->val = (char)cur->val;
      p += cur->len;
      continue;
    }
    // UTF-16 character literal
    if (LOOKINGAT(p, "u'")) {
      cur = cur->next = read_char_literal(p, p + 1, ty_ushort);
      cur->val &= 0xffff;
      p += cur->len;
      continue;
    }
    // Wide character literal
    if (LOOKINGAT(p, "L'")) {
      cur = cur->next = read_char_literal(p, p + 1, ty_int);
      p += cur->len;
      continue;
    }
    // UTF-32 character literal
    if (LOOKINGAT(p, "U'")) {
      cur = cur->next = read_char_literal(p, p + 1, ty_uint);
      p += cur->len;
      continue;
    }
    // Identifier or keyword
    int ident_len = read_ident(p);
    if (ident_len) {
      cur = cur->next = new_token(TK_IDENT, p, p + ident_len);
      p += cur->len;
      continue;
    }
    // Punctuators
    int punct_len = read_punct(p);
    if (punct_len) {
      cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
      p += cur->len;
      continue;
    }
    error_at(p, "invalid token");
  }
  cur = cur->next = new_token(TK_EOF, p, p);
  add_line_numbers(head.next);
  return head.next;
}

File **get_input_files(void) {
  return input_files;
}

File *new_file(char *name, int file_no, char *contents) {
  File *file = calloc(1, sizeof(File));
  file->name = name;
  file->display_name = name;
  file->file_no = file_no;
  file->contents = contents;
  return file;
}

static uint32_t read_universal_char(char *p, int len) {
  uint32_t c = 0;
  for (int i = 0; i < len; i++) {
    if (!isxdigit(p[i])) return 0;
    c = (c << 4) | from_hex(p[i]);
  }
  return c;
}

// Replace \u or \U escape sequences with corresponding UTF-8 bytes.
static void convert_universal_chars(char *p) {
  char *q = p;
  while (*p) {
    if (LOOKINGAT(p, "\\u")) {
      uint32_t c = read_universal_char(p + 2, 4);
      if (c) {
        p += 6;
        q += encode_utf8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (LOOKINGAT(p, "\\U")) {
      uint32_t c = read_universal_char(p + 2, 8);
      if (c) {
        p += 10;
        q += encode_utf8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (p[0] == '\\') {
      *q++ = *p++;
      *q++ = *p++;
    } else {
      *q++ = *p++;
    }
  }
  *q = '\0';
}

Token *tokenize_file(char *path) {
  char *p = read_file(path);
  if (!p) return NULL;
  p = skip_bom(p);
  canonicalize_newline(p);
  remove_backslash_newline(p);
  convert_universal_chars(p);
  // Save the filename for assembler .file directive.
  static int file_no;
  File *file = new_file(path, file_no + 1, p);
  // Save the filename for assembler .file directive.
  input_files = realloc(input_files, sizeof(char *) * (file_no + 2));
  input_files[file_no] = file;
  input_files[file_no + 1] = NULL;
  file_no++;
  return tokenize(file);
}
