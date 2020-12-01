#ifndef COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_
#define COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _POSIX_C_SOURCE 200809L

#include "libc/assert.h"
#include "libc/bits/popcnt.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/weirdtypes.h"
#include "libc/conv/conv.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/unicode/unicode.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"

#pragma GCC diagnostic ignored "-Wswitch"

#ifndef __GNUC__
#define __attribute__(x)
#endif

typedef struct Type Type;
typedef struct Node Node;
typedef struct Member Member;
typedef struct Relocation Relocation;
typedef struct Hideset Hideset;

//
// strarray.c
//

typedef struct {
  char **data;
  int capacity;
  int len;
} StringArray;

void strarray_push(StringArray *arr, char *s);

//
// tokenize.c
//

// Token
typedef enum {
  TK_RESERVED,  // Keywords or punctuators
  TK_IDENT,     // Identifiers
  TK_STR,       // String literals
  TK_NUM,       // Numeric literals
  TK_PP_NUM,    // Preprocessing numbers
  TK_EOF,       // End-of-file markers
} TokenKind;

typedef struct {
  char *name;
  int file_no;
  char *contents;

  // For #line directive
  char *display_name;
  int line_delta;
} File;

// Token type
typedef struct Token Token;
struct Token {
  TokenKind kind;    // Token kind
  Token *next;       // Next token
  int64_t val;       // If kind is TK_NUM, its value
  long double fval;  // If kind is TK_NUM, its value
  char *loc;         // Token location
  int len;           // Token length
  Type *ty;          // Used if TK_NUM or TK_STR
  char *str;         // String literal contents including terminating '\0'

  File *file;        // Source location
  char *filename;    // Filename
  int line_no;       // Line number
  int line_delta;    // Line number
  bool at_bol;       // True if this token is at beginning of line
  bool has_space;    // True if this token follows a space character
  Hideset *hideset;  // For macro expansion
  Token *origin;     // If this is expanded from a macro, the original token
};

noreturn void error(char *fmt, ...) __attribute__((format(printf, 1, 2)));
noreturn void error_at(char *loc, char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
noreturn void error_tok(Token *tok, char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
void warn_tok(Token *tok, char *fmt, ...) __attribute__((format(printf, 2, 3)));
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
void convert_pp_tokens(Token *tok);
File **get_input_files(void);
File *new_file(char *name, int file_no, char *contents);
Token *tokenize_string_literal(Token *tok, Type *basety);
Token *tokenize(File *file);
Token *tokenize_file(char *filename);

#define UNREACHABLE() error("internal error at %s:%d", __FILE__, __LINE__)

//
// preprocess.c
//

char *format(char *fmt, ...);
char *search_include_paths(char *filename);
bool file_exists(char *path);
void init_macros(void);
void define_macro(char *name, char *buf);
void undef_macro(char *name);
Token *preprocess(Token *tok);

//
// parse.c
//

// Variable or function
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name;     // Variable name
  Type *ty;       // Type
  Token *tok;     // representative token
  bool is_local;  // local or global/function
  int align;      // alignment

  // Local variable
  int offset;

  // Global variable or function
  bool is_function;
  bool is_definition;
  bool is_static;

  // Global variable
  bool is_tentative;
  bool is_tls;
  char *init_data;
  Relocation *rel;

  // Function
  bool is_inline;
  Obj *params;
  Node *body;
  Obj *locals;
  Obj *va_area;
  Obj *alloca_bottom;
  int stack_size;

  // Static inline function
  bool is_live;
  bool is_root;
  StringArray refs;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.
typedef struct Relocation Relocation;
struct Relocation {
  Relocation *next;
  int offset;
  char **label;
  long addend;
};

// AST node
typedef enum {
  ND_NULL_EXPR,  // Do nothing
  ND_ADD,        // +
  ND_SUB,        // -
  ND_MUL,        // *
  ND_DIV,        // /
  ND_NEG,        // unary -
  ND_MOD,        // %
  ND_BITAND,     // &
  ND_BITOR,      // |
  ND_BITXOR,     // ^
  ND_SHL,        // <<
  ND_SHR,        // >>
  ND_EQ,         // ==
  ND_NE,         // !=
  ND_LT,         // <
  ND_LE,         // <=
  ND_ASSIGN,     // =
  ND_COND,       // ?:
  ND_COMMA,      // ,
  ND_MEMBER,     // . (struct member access)
  ND_ADDR,       // unary &
  ND_DEREF,      // unary *
  ND_NOT,        // !
  ND_BITNOT,     // ~
  ND_LOGAND,     // &&
  ND_LOGOR,      // ||
  ND_RETURN,     // "return"
  ND_IF,         // "if"
  ND_FOR,        // "for" or "while"
  ND_DO,         // "do"
  ND_SWITCH,     // "switch"
  ND_CASE,       // "case"
  ND_BLOCK,      // { ... }
  ND_GOTO,       // "goto"
  ND_GOTO_EXPR,  // "goto" labels-as-values
  ND_LABEL,      // Labeled statement
  ND_LABEL_VAL,  // [GNU] Labels-as-values
  ND_FUNCALL,    // Function call
  ND_EXPR_STMT,  // Expression statement
  ND_STMT_EXPR,  // Statement expression
  ND_VAR,        // Variable
  ND_VLA_PTR,    // VLA designator
  ND_NUM,        // Integer
  ND_CAST,       // Type cast
  ND_MEMZERO,    // Zero-clear a stack variable
  ND_ASM,        // "asm"
  ND_CAS,        // Atomic compare-and-swap
  ND_EXCH,       // Atomic exchange
} NodeKind;

// AST node type
struct Node {
  NodeKind kind;  // Node kind
  Node *next;     // Next node
  Type *ty;       // Type, e.g. int or pointer to int
  Token *tok;     // Representative token

  Node *lhs;  // Left-hand side
  Node *rhs;  // Right-hand side

  // "if" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // "break" and "continue" labels
  char *brk_label;
  char *cont_label;

  // Block or statement expression
  Node *body;

  // Struct member access
  Member *member;

  // Function call
  Type *func_ty;
  Node *args;
  bool pass_by_stack;
  Obj *ret_buffer;

  // Goto or labeled statement, or labels-as-values
  char *label;
  char *unique_label;
  Node *goto_next;

  // Switch
  Node *case_next;
  Node *default_case;

  // Case
  long begin;
  long end;

  // "asm" string literal
  char *asm_str;

  // Atomic compare-and-swap
  Node *cas_addr;
  Node *cas_old;
  Node *cas_new;

  // Atomic op= operators
  Obj *atomic_addr;
  Node *atomic_expr;

  // Variable
  Obj *var;

  // Numeric literal
  int64_t val;
  long double fval;
};

Node *new_cast(Node *expr, Type *ty);
int64_t const_expr(Token **rest, Token *tok);
Obj *parse(Token *tok);

//
// type.c
//

typedef enum {
  TY_VOID,
  TY_BOOL,
  TY_CHAR,
  TY_SHORT,
  TY_INT,
  TY_LONG,
  TY_FLOAT,
  TY_DOUBLE,
  TY_LDOUBLE,
  TY_ENUM,
  TY_PTR,
  TY_FUNC,
  TY_ARRAY,
  TY_VLA,  // variable-length array
  TY_STRUCT,
  TY_UNION,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;          // sizeof() value
  int align;         // alignment
  bool is_unsigned;  // unsigned or signed
  bool is_atomic;    // true if _Atomic
  Type *origin;      // for type compatibility check

  // Pointer-to or array-of type. We intentionally use the same member
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // member instead of "kind" member to determine whether a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  Type *base;

  // Declaration
  Token *name;
  Token *name_pos;

  // Array
  int array_len;

  // Variable-length array
  Node *vla_len;  // # of elements
  Obj *vla_size;  // sizeof() value

  // Struct
  Member *members;
  bool is_flexible;
  bool is_packed;

  // Function type
  Type *return_ty;
  Type *params;
  bool is_variadic;
  Type *next;
};

// Struct member
struct Member {
  Member *next;
  Type *ty;
  Token *tok;  // for error message
  Token *name;
  int idx;
  int align;
  int offset;

  // Bitfield
  bool is_bitfield;
  int bit_offset;
  int bit_width;
};

extern Type *ty_void;
extern Type *ty_bool;

extern Type *ty_char;
extern Type *ty_short;
extern Type *ty_int;
extern Type *ty_long;

extern Type *ty_uchar;
extern Type *ty_ushort;
extern Type *ty_uint;
extern Type *ty_ulong;

extern Type *ty_float;
extern Type *ty_double;
extern Type *ty_ldouble;

bool is_integer(Type *ty);
bool is_flonum(Type *ty);
bool is_numeric(Type *ty);
bool is_compatible(Type *t1, Type *t2);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int size);
Type *vla_of(Type *base, Node *expr);
Type *enum_type(void);
Type *struct_type(void);
void add_type(Node *node);

//
// codegen.c
//

void codegen(Obj *prog, FILE *out);
int align_to(int n, int align);

//
// unicode.c
//

int encode_utf8(char *buf, uint32_t c);
uint32_t decode_utf8(char **new_pos, char *p);
bool is_ident1(uint32_t c);
bool is_ident2(uint32_t c);
int str_width(char *p, int len);

//
// hashmap.c
//

typedef struct {
  char *key;
  int keylen;
  void *val;
} HashEntry;

typedef struct {
  HashEntry *buckets;
  int capacity;
  int used;
} HashMap;

void *hashmap_get(HashMap *map, char *key);
void *hashmap_get2(HashMap *map, char *key, int keylen);
void hashmap_put(HashMap *map, char *key, void *val);
void hashmap_put2(HashMap *map, char *key, int keylen, void *val);
void hashmap_delete(HashMap *map, char *key);
void hashmap_delete2(HashMap *map, char *key, int keylen);
void hashmap_test(void);

//
// main.c
//

extern StringArray include_paths;
extern bool opt_fpic;
extern bool opt_fcommon;
extern char *base_file;

typedef struct StaticAsm {
  struct StaticAsm *next;
  Node *body;
} StaticAsm;

extern struct StaticAsm *staticasms;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_ */
