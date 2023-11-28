#ifndef COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_
#define COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/popcnt.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/str/str.h"
#include "libc/str/unicode.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "tool/build/lib/javadown.h"
COSMOPOLITAN_C_START_

#pragma GCC diagnostic ignored "-Wswitch"

typedef struct Asm Asm;
typedef struct AsmOperand AsmOperand;
typedef struct File File;
typedef struct FpClassify FpClassify;
typedef struct HashMap HashMap;
typedef struct Hideset Hideset;
typedef struct Macro Macro;
typedef struct MacroParam MacroParam;
typedef struct Member Member;
typedef struct Node Node;
typedef struct Obj Obj;
typedef struct Relocation Relocation;
typedef struct Relocation Relocation;
typedef struct StaticAsm StaticAsm;
typedef struct StringArray StringArray;
typedef struct Token Token;
typedef struct TokenStack TokenStack;
typedef struct Type Type;

typedef Token *macro_handler_fn(Token *);

//
// strarray.c
//

struct StringArray {
  char **data;
  int capacity;
  int len;
};

void strarray_push(StringArray *, char *);

//
// tokenize.c
//

typedef enum {
  TK_IDENT,     // Identifiers
  TK_PUNCT,     // Punctuators
  TK_KEYWORD,   // Keywords
  TK_STR,       // String literals
  TK_NUM,       // Numeric literals
  TK_PP_NUM,    // Preprocessing numbers
  TK_JAVADOWN,  // /** ... */ comments
  TK_EOF,       // End-of-file markers
} TokenKind;

struct File {
  char *name;
  int file_no;
  char *contents;
  // For #line directive
  char *display_name;
  int line_delta;
  struct Javadown *javadown;
};

struct thatispacked Token {
  Token *next;       // Next token
  int len;           // Token length
  int line_no;       // Line number
  int line_delta;    // Line number
  TokenKind kind;    // Token kind
  uint8_t kw;        // Keyword Phash
  bool at_bol;       // True if this token is at beginning of line
  bool has_space;    // True if this token follows a space character
  char *loc;         // Token location
  Type *ty;          // Used if TK_NUM or TK_STR
  File *file;        // Source location
  char *filename;    // Filename
  Hideset *hideset;  // For macro expansion
  Token *origin;     // If this is expanded from a macro, the original token
  struct Javadown *javadown;
  union {
    int64_t val;       // If kind is TK_NUM, its value
    long double fval;  // If kind is TK_NUM, its value
    char *str;         // String literal contents including terminating '\0'
  };
};

void error(char *, ...)
    __attribute__((__noreturn__, __format__(__printf__, 1, 2)));
void error_at(char *, char *, ...)
    __attribute__((__noreturn__, __format__(__printf__, 2, 3)));
void error_tok(Token *, char *, ...)
    __attribute__((__noreturn__, __format__(__printf__, 2, 3)));
void warn_tok(Token *, char *, ...)
    __attribute__((__format__(__printf__, 2, 3)));

File **get_input_files(void);
File *new_file(char *, int, char *);
Token *skip(Token *, char);
Token *tokenize(File *);
Token *tokenize_file(char *);
Token *tokenize_string_literal(Token *, Type *);
bool consume(Token **, Token *, char *, size_t);
bool equal(Token *, char *, size_t);
void convert_pp_tokens(Token *);
int read_escaped_char(char **, char *);

#define UNREACHABLE()    error("internal error at %s:%d", __FILE__, __LINE__)
#define EQUAL(T, S)      equal(T, S, sizeof(S) - 1)
#define CONSUME(R, T, S) consume(R, T, S, sizeof(S) - 1)

//
// preprocess.c
//

struct MacroParam {
  MacroParam *next;
  char *name;
};

struct Macro {
  char *name;
  bool is_objlike;  // Object-like or function-like
  MacroParam *params;
  char *va_args_name;
  Token *body;
  macro_handler_fn *handler;
  Token *javadown;
};

extern HashMap macros;

char *search_include_paths(char *);
void init_macros(void);
void init_macros_conditional(void);
void define_macro(char *, char *);
void undef_macro(char *);
Token *preprocess(Token *);

//
// asm.c
//

#define kAsmImm  1
#define kAsmMem  2
#define kAsmReg  4
#define kAsmMmx  8
#define kAsmXmm  16
#define kAsmFpu  32
#define kAsmRaw  64
#define kAsmFlag 128

struct AsmOperand {
  uint8_t reg;
  uint8_t type;
  char flow;
  unsigned char x87mask;
  bool isused;
  int regmask;
  int predicate;
  char *str;
  Node *node;
  Token *tok;
  int64_t val;
  char **label;
};

struct Asm {
  int n;
  char *str;
  Token *tok;
  AsmOperand ops[20];
  bool isgnu;
  bool flagclob;
  uint8_t x87clob;
  uint16_t regclob;
  uint16_t xmmclob;
};

struct StaticAsm {
  StaticAsm *next;
  Asm *body;
};

extern StaticAsm *staticasms;

Asm *asm_stmt(Token **, Token *);
void flushln(void);
void gen_addr(Node *);
void gen_asm(Asm *);
void gen_expr(Node *);
void pop(char *);
void popreg(const char *);
void print_loc(int64_t, int64_t);
void push(void);
void pushreg(const char *);

//
// fpclassify.c
//

struct FpClassify {
  Node *node;
  int args[5];
};

void gen_fpclassify(FpClassify *);

//
// parse.c
//

// Variable or function
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
  bool is_weak;
  bool is_externally_visible;
  char *asmname;
  char *section;
  char *visibility;
  Token *javadown;
  // Global variable
  bool is_tentative;
  bool is_string_literal;
  bool is_tls;
  char *init_data;
  Relocation *rel;
  // Function
  bool is_inline;
  bool is_aligned;
  bool is_noreturn;
  bool is_destructor;
  bool is_constructor;
  bool is_ms_abi; /* TODO */
  bool is_no_instrument_function;
  bool is_force_align_arg_pointer;
  bool is_no_caller_saved_registers;
  int stack_size;
  Obj *params;
  Node *body;
  Obj *locals;
  Obj *va_area;
  Obj *alloca_bottom;
  // Dead Code Elimination
  bool is_live;
  bool is_root;
  StringArray refs;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.
struct Relocation {
  Relocation *next;
  int offset;
  char **label;
  long addend;
};

typedef enum {
  ND_NULL_EXPR,    // Do nothing
  ND_ADD,          // +
  ND_SUB,          // -
  ND_MUL,          // *
  ND_DIV,          // /
  ND_NEG,          // unary -
  ND_REM,          // %
  ND_BINAND,       // &
  ND_BINOR,        // |
  ND_BINXOR,       // ^
  ND_SHL,          // <<
  ND_SHR,          // >>
  ND_EQ,           // ==
  ND_NE,           // !=
  ND_LT,           // <
  ND_LE,           // <=
  ND_ASSIGN,       // =
  ND_COND,         // ?:
  ND_COMMA,        // ,
  ND_MEMBER,       // . (struct member access)
  ND_ADDR,         // unary &
  ND_DEREF,        // unary *
  ND_NOT,          // !
  ND_BITNOT,       // ~
  ND_LOGAND,       // &&
  ND_LOGOR,        // ||
  ND_RETURN,       // "return"
  ND_IF,           // "if"
  ND_FOR,          // "for" or "while"
  ND_DO,           // "do"
  ND_SWITCH,       // "switch"
  ND_CASE,         // "case"
  ND_BLOCK,        // { ... }
  ND_GOTO,         // "goto"
  ND_GOTO_EXPR,    // "goto" labels-as-values
  ND_LABEL,        // Labeled statement
  ND_LABEL_VAL,    // [GNU] Labels-as-values
  ND_FUNCALL,      // Function call
  ND_EXPR_STMT,    // Expression statement
  ND_STMT_EXPR,    // Statement expression
  ND_VAR,          // Variable
  ND_VLA_PTR,      // VLA designator
  ND_NUM,          // Integer
  ND_CAST,         // Type cast
  ND_MEMZERO,      // Zero-clear a stack variable
  ND_ASM,          // "asm"
  ND_CAS,          // Atomic compare-and-swap
  ND_EXCH_N,       // Atomic exchange with value
  ND_LOAD,         // Atomic load to pointer
  ND_LOAD_N,       // Atomic load to result
  ND_STORE,        // Atomic store to pointer
  ND_STORE_N,      // Atomic store to result
  ND_TESTANDSET,   // Sync lock test and set
  ND_TESTANDSETA,  // Atomic lock test and set
  ND_CLEAR,        // Atomic clear
  ND_RELEASE,      // Atomic lock release
  ND_FETCHADD,     // Atomic fetch and add
  ND_FETCHSUB,     // Atomic fetch and sub
  ND_FETCHXOR,     // Atomic fetch and xor
  ND_FETCHAND,     // Atomic fetch and and
  ND_FETCHOR,      // Atomic fetch and or
  ND_SUBFETCH,     // Atomic sub and fetch
  ND_FPCLASSIFY,   // floating point classify
  ND_MOVNTDQ,      // Intel MOVNTDQ
  ND_PMOVMSKB,     // Intel PMOVMSKB
} NodeKind;

struct Node {
  NodeKind kind;  // Node kind
  Node *next;     // Next node
  Type *ty;       // Type, e.g. int or pointer to int
  Token *tok;     // Representative token
  Node *lhs;      // Left-hand side
  Node *rhs;      // Right-hand side
  // "if" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;
  // Block or statement expression
  Node *body;
  // Function call
  Type *func_ty;
  Node *args;
  Obj *ret_buffer;
  bool pass_by_stack;
  bool realign_stack;
  // Switch
  Node *case_next;
  Node *default_case;
  // Goto or labeled statement, or labels-as-values
  char *label;
  char *unique_label;
  Node *goto_next;
  // "break" and "continue" labels
  char *brk_label;
  char *cont_label;
  // Case
  long begin;
  long end;
  // Struct member access
  Member *member;
  // Assembly
  Asm *azm;
  // Atomic compare-and-swap
  char memorder;
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
  FpClassify *fpc;
};

Node *expr(Token **, Token *);
Node *new_cast(Node *, Type *);
Node *new_node(NodeKind, Token *);
Obj *parse(Token *);
bool is_const_expr(Node *);
char *ConsumeStringLiteral(Token **, Token *);
int64_t const_expr(Token **, Token *);
int64_t eval(Node *);
int64_t eval2(Node *, char ***);

//
// debug.c
//

void print_ast(FILE *, Obj *);

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
  TY_INT128,
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
  bool is_const;     // const
  bool is_restrict;  // restrict
  bool is_volatile;  // volatile
  bool is_ms_abi;    // microsoft abi
  bool is_static;    // for array parameter pointer
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
  // Array or decayed pointer
  int array_len;
  int vector_size;
  // Variable-length array
  Node *vla_len;  // # of elements
  Obj *vla_size;  // sizeof() value
  // Struct
  Member *members;
  bool is_flexible;
  bool is_packed;
  bool is_aligned;
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

extern Type ty_void[1];
extern Type ty_bool[1];
extern Type ty_char[1];
extern Type ty_short[1];
extern Type ty_int[1];
extern Type ty_long[1];
extern Type ty_int128[1];
extern Type ty_uchar[1];
extern Type ty_ushort[1];
extern Type ty_uint[1];
extern Type ty_ulong[1];
extern Type ty_uint128[1];
extern Type ty_float[1];
extern Type ty_double[1];
extern Type ty_ldouble[1];

bool is_integer(Type *);
bool is_flonum(Type *);
bool is_numeric(Type *);
bool is_compatible(Type *, Type *);
Type *copy_type(Type *);
Type *pointer_to(Type *);
Type *func_type(Type *);
Type *array_of(Type *, int);
Type *vla_of(Type *, Node *);
Type *enum_type(void);
Type *struct_type(void);
void add_type(Node *);

//
// cast.c
//

void gen_cast(Type *, Type *);

//
// codegen.c
//

extern int depth;
extern FILE *output_stream;

int align_to(int, int);
int count(void);
void cmp_zero(Type *);
void codegen(Obj *, FILE *);
void gen_stmt(Node *);
void println(char *, ...) __attribute__((__format__(__printf__, 1, 2)));

//
// unicode.c
//

int encode_utf8(char *, uint32_t);
uint32_t decode_utf8(char **, char *);
bool is_ident1(uint32_t);
bool is_ident2(uint32_t);
int display_width(char *, int);

//
// hashmap.c
//

typedef struct {
  char *key;
  int keylen;
  void *val;
} HashEntry;

struct HashMap {
  HashEntry *buckets;
  int capacity;
  int used;
};

extern long chibicc_hashmap_hits;
extern long chibicc_hashmap_miss;

void *hashmap_get(HashMap *, char *);
void *hashmap_get2(HashMap *, char *, int);
void hashmap_put(HashMap *, char *, void *);
void hashmap_put2(HashMap *, char *, int, void *);
void hashmap_delete(HashMap *, char *);
void hashmap_delete2(HashMap *, char *, int);
void hashmap_test(void);

//
// chibicc.c
//

extern StringArray include_paths;
extern bool opt_common;
extern bool opt_data_sections;
extern bool opt_fentry;
extern bool opt_function_sections;
extern bool opt_no_builtin;
extern bool opt_nop_mcount;
extern bool opt_pg;
extern bool opt_pic;
extern bool opt_popcnt;
extern bool opt_record_mcount;
extern bool opt_sse3;
extern bool opt_sse4;
extern bool opt_verbose;
extern char *base_file;
extern char **chibicc_tmpfiles;

int chibicc(int, char **);
void chibicc_cleanup(void);

//
// alloc.c
//

extern long alloc_node_count;
extern long alloc_token_count;
extern long alloc_obj_count;
extern long alloc_type_count;

Node *alloc_node(void);
Token *alloc_token(void);
Obj *alloc_obj(void);
Type *alloc_type(void);

//
// dox1.c
//

void output_javadown(const char *, Obj *);
void output_javadown_asm(const char *, const char *);

//
// dox2.c
//

void drop_dox(const StringArray *, const char *);

//
// as.c
//

extern long as_hashmap_hits;
extern long as_hashmap_miss;

void Assembler(int, char **);

//
// pybind.c
//
void output_bindings_python(const char *, Obj *, Token *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_CHIBICC_CHIBICC_H_ */
