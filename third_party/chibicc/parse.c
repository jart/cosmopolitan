// This file contains a recursive descent parser for C.
//
// Most functions in this file are named after the symbols they are
// supposed to read from an input token list. For example, stmt() is
// responsible for reading a statement from a token list. The function
// then construct an AST node representing a statement.
//
// Each function conceptually returns two values, an AST node and
// remaining part of the input tokens. Since C doesn't support
// multiple return values, the remaining tokens are returned to the
// caller via a pointer argument.
//
// Input tokens are represented by a linked list. Unlike many recursive
// descent parsers, we don't have the notion of the "input token stream".
// Most parsing functions don't change the global state of the parser.
// So it is very easy to lookahead arbitrary number of tokens in this
// parser.

#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/ffs.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "third_party/chibicc/chibicc.h"
#include "third_party/chibicc/kw.h"

#define IMPLICIT_FUNCTIONS

typedef struct InitDesg InitDesg;
typedef struct Initializer Initializer;
typedef struct Scope Scope;

// Scope for local variables, global variables, typedefs
// or enum constants
typedef struct {
  Obj *var;
  Type *type_def;
  Type *enum_ty;
  int enum_val;
} VarScope;

// Represents a block scope.
struct Scope {
  Scope *next;
  // C has two block scopes; one is for variables/typedefs and
  // the other is for struct/union/enum tags.
  HashMap vars;
  HashMap tags;
};

// Variable attributes such as typedef or extern.
typedef struct {
  bool is_typedef;
  bool is_static;
  bool is_extern;
  bool is_inline;
  bool is_tls;
  bool is_weak;
  bool is_ms_abi;
  bool is_aligned;
  bool is_noreturn;
  bool is_destructor;
  bool is_constructor;
  bool is_externally_visible;
  bool is_no_instrument_function;
  bool is_force_align_arg_pointer;
  bool is_no_caller_saved_registers;
  int align;
  char *section;
  char *visibility;
} VarAttr;

// This struct represents a variable initializer. Since initializers
// can be nested (e.g. `int x[2][2] = {{1, 2}, {3, 4}}`), this struct
// is a tree data structure.
struct Initializer {
  Initializer *next;
  Type *ty;
  Token *tok;
  bool is_flexible;
  // If it's not an aggregate type and has an initializer,
  // `expr` has an initialization expression.
  Node *expr;
  // If it's an initializer for an aggregate type (e.g. array or struct),
  // `children` has initializers for its children.
  Initializer **children;
  // Only one member can be initialized for a union.
  // `mem` is used to clarify which member is initialized.
  Member *mem;
};

// For local variable initializer.
struct InitDesg {
  InitDesg *next;
  int idx;
  Member *member;
  Obj *var;
};

// All local variable instances created during parsing are
// accumulated to this list.
static Obj *locals;

// Likewise, global variables are accumulated to this list.
static Obj *globals;

static Scope *scope = &(Scope){};

// Points to the function object the parser is currently parsing.
static Obj *current_fn;

// Lists of all goto statements and labels in the curent function.
static Node *gotos;
static Node *labels;

// Current "goto" and "continue" jump targets.
static char *brk_label;
static char *cont_label;

// Points to a node representing a switch if we are parsing
// a switch statement. Otherwise, NULL.
static Node *current_switch;

static Obj *builtin_alloca;

static Token *current_javadown;

static Initializer *initializer(Token **, Token *, Type *, Type **);
static Member *get_struct_member(Type *, Token *);
static Node *binor(Token **, Token *);
static Node *add(Token **, Token *);
static Node *assign(Token **, Token *);
static Node *binand(Token **, Token *);
static Node *binxor(Token **, Token *);
static Node *cast(Token **, Token *);
static Node *compound_stmt(Token **, Token *);
static Node *conditional(Token **, Token *);
static Node *declaration(Token **, Token *, Type *, VarAttr *);
static Node *equality(Token **, Token *);
static Node *expr_stmt(Token **, Token *);
static Node *funcall(Token **, Token *, Node *);
static Node *logand(Token **, Token *);
static Node *logor(Token **, Token *);
static Node *lvar_initializer(Token **, Token *, Obj *);
static Node *mul(Token **, Token *);
static Node *new_add(Node *, Node *, Token *);
static Node *new_sub(Node *, Node *, Token *);
static Node *postfix(Token **, Token *);
static Node *primary(Token **, Token *);
static Node *relational(Token **, Token *);
static Node *shift(Token **, Token *);
static Node *stmt(Token **, Token *);
static Node *unary(Token **, Token *);
static Token *function(Token *, Type *, VarAttr *);
static Token *global_variable(Token *, Type *, VarAttr *);
static Token *parse_typedef(Token *, Type *);
static Type *declarator(Token **, Token *, Type *);
static Type *enum_specifier(Token **, Token *);
static Type *struct_decl(Token **, Token *);
static Type *type_suffix(Token **, Token *, Type *);
static Type *typename(Token **, Token *);
static Type *typeof_specifier(Token **, Token *);
static Type *union_decl(Token **, Token *);
static bool is_function(Token *);
static bool is_typename(Token *);
static double eval_double(Node *);
static int64_t eval_rval(Node *, char ***);
static void array_initializer2(Token **, Token *, Initializer *, int);
static void gvar_initializer(Token **, Token *, Obj *);
static void initializer2(Token **, Token *, Initializer *);
static void struct_initializer2(Token **, Token *, Initializer *, Member *);

static void enter_scope(void) {
  Scope *sc = calloc(1, sizeof(Scope));
  sc->next = scope;
  scope = sc;
}

static void leave_scope(void) {
  scope = scope->next;
}

// Find a variable by name.
static VarScope *find_var(Token *tok) {
  for (Scope *sc = scope; sc; sc = sc->next) {
    VarScope *sc2 = hashmap_get2(&sc->vars, tok->loc, tok->len);
    if (sc2) return sc2;
  }
  return NULL;
}

static Type *find_tag(Token *tok) {
  for (Scope *sc = scope; sc; sc = sc->next) {
    Type *ty = hashmap_get2(&sc->tags, tok->loc, tok->len);
    if (ty) return ty;
  }
  return NULL;
}

Node *new_node(NodeKind kind, Token *tok) {
  Node *node = alloc_node();
  node->kind = kind;
  node->tok = tok;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

static Node *new_num(int64_t val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);
  node->val = val;
  return node;
}

static Node *new_int(int64_t val, Token *tok) {
  Node *node = new_num(val, tok);
  node->ty = ty_int;
  return node;
}

static Node *new_bool(int64_t val, Token *tok) {
  Node *node = new_num(val, tok);
  node->ty = ty_bool;
  return node;
}

static Node *new_long(int64_t val, Token *tok) {
  Node *node = new_num(val, tok);
  node->ty = ty_long;
  return node;
}

static Node *new_ulong(int64_t val, Token *tok) {
  Node *node = new_num(val, tok);
  node->ty = ty_ulong;
  return node;
}

static Node *new_var_node(Obj *var, Token *tok) {
  CHECK_NOTNULL(var);
  Node *node = new_node(ND_VAR, tok);
  node->var = var;
  return node;
}

static Node *new_vla_ptr(Obj *var, Token *tok) {
  Node *node = new_node(ND_VLA_PTR, tok);
  node->var = var;
  return node;
}

Node *new_cast(Node *expr, Type *ty) {
  add_type(expr);
  Node *node = alloc_node();
  node->kind = ND_CAST;
  node->tok = expr->tok;
  node->lhs = expr;
  node->ty = copy_type(ty);
  return node;
}

static VarScope *push_scope(char *name) {
  VarScope *sc = calloc(1, sizeof(VarScope));
  hashmap_put(&scope->vars, name, sc);
  return sc;
}

static Initializer *new_initializer(Type *ty, bool is_flexible) {
  Initializer *init = calloc(1, sizeof(Initializer));
  init->ty = ty;
  if (ty->kind == TY_ARRAY) {
    if (is_flexible && ty->size < 0) {
      init->is_flexible = true;
      return init;
    }
    init->children = calloc(ty->array_len, sizeof(Initializer *));
    for (int i = 0; i < ty->array_len; i++) {
      init->children[i] = new_initializer(ty->base, false);
    }
    return init;
  }
  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
    // Count the number of struct members.
    int len = 0;
    for (Member *mem = ty->members; mem; mem = mem->next) len++;
    init->children = calloc(len, sizeof(Initializer *));
    for (Member *mem = ty->members; mem; mem = mem->next) {
      if (is_flexible && ty->is_flexible && !mem->next) {
        Initializer *child = calloc(1, sizeof(Initializer));
        child->ty = mem->ty;
        child->is_flexible = true;
        init->children[mem->idx] = child;
      } else {
        init->children[mem->idx] = new_initializer(mem->ty, false);
      }
    }
    return init;
  }
  return init;
}

static Obj *new_var(char *name, Type *ty) {
  Obj *var = alloc_obj();
  var->name = name;
  var->ty = ty;
  var->align = ty->align;
  push_scope(name)->var = var;
  return var;
}

static Obj *new_lvar(char *name, Type *ty) {
  Obj *var = new_var(name, ty);
  var->is_local = true;
  var->next = locals;
  locals = var;
  return var;
}

static Obj *new_gvar(char *name, Type *ty) {
  Obj *var = new_var(name, ty);
  var->next = globals;
  var->is_static = true;
  var->is_definition = true;
  globals = var;
  return var;
}

static char *new_unique_name(void) {
  static int id = 0;
  return xasprintf(".L..%d", id++);
}

static Obj *new_anon_gvar(Type *ty) {
  return new_gvar(new_unique_name(), ty);
}

static Obj *new_string_literal(char *p, Type *ty) {
  Obj *var = new_anon_gvar(ty);
  var->init_data = p;
  var->is_string_literal = true;
  return var;
}

static char *get_ident(Token *tok) {
  if (tok->kind != TK_IDENT) {
    __die();
    error_tok(tok, "expected an identifier");
  }
  return strndup(tok->loc, tok->len);
}

static Type *find_typedef(Token *tok) {
  if (tok->kind == TK_IDENT) {
    VarScope *sc = find_var(tok);
    if (sc) return sc->type_def;
  }
  return NULL;
}

static void push_tag_scope(Token *tok, Type *ty) {
  hashmap_put2(&scope->tags, tok->loc, tok->len, ty);
}

// Consumes token if equal to STR or __STR__.
static bool consume_attribute(Token **rest, Token *tok, char *name) {
  size_t n = strlen(name);
  if ((n == tok->len && !memcmp(tok->loc, name, n)) ||
      (2 + n + 2 == tok->len && tok->loc[0] == '_' && tok->loc[1] == '_' &&
       tok->loc[tok->len - 2] == '_' && tok->loc[tok->len - 1] == '_' &&
       !memcmp(tok->loc + 2, name, n))) {
    *rest = tok->next;
    return true;
  }
  *rest = tok;
  return false;
}

static Token *attribute_list(Token *tok, void *arg,
                             Token *(*f)(Token *, void *)) {
  while (CONSUME(&tok, tok, "__attribute__")) {
    tok = skip(tok, '(');
    tok = skip(tok, '(');
    bool first = true;
    while (!CONSUME(&tok, tok, ")")) {
      if (!first) tok = skip(tok, ',');
      first = false;
      tok = f(tok, arg);
    }
    tok = skip(tok, ')');
  }
  return tok;
}

static Token *type_attributes(Token *tok, void *arg) {
  Type *ty = arg;
  if (consume_attribute(&tok, tok, "packed")) {
    ty->is_packed = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "ms_abi")) {
    ty->is_ms_abi = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "aligned")) {
    ty->is_aligned = true;
    if (CONSUME(&tok, tok, "(")) {
      Token *altok = tok;
      ty->align = const_expr(&tok, tok);
      if (popcnt(ty->align) != 1) error_tok(altok, "must be two power");
      tok = skip(tok, ')');
    } else {
      ty->align = 16; /* biggest alignment */
    }
    return tok;
  }
  if (consume_attribute(&tok, tok, "vector_size")) {
    tok = skip(tok, '(');
    int vs = const_expr(&tok, tok);
    if (vs != 16) {
      error_tok(tok, "only vector_size 16 supported");
    }
    if (vs != ty->vector_size) {
      ty->size = vs;
      ty->vector_size = vs;
      if (!ty->is_aligned) ty->align = vs;
      /* ty->base = ty; */
      /* ty->array_len = vs / ty->size; */
    }
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "warn_if_not_aligned")) {
    tok = skip(tok, '(');
    const_expr(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "deprecated") ||
      consume_attribute(&tok, tok, "may_alias") ||
      consume_attribute(&tok, tok, "unused")) {
    return tok;
  }
  if (consume_attribute(&tok, tok, "alloc_size") ||
      consume_attribute(&tok, tok, "alloc_align")) {
    if (CONSUME(&tok, tok, "(")) {
      const_expr(&tok, tok);
      tok = skip(tok, ')');
    }
    return tok;
  }
  error_tok(tok, "unknown type attribute");
}

static Token *thing_attributes(Token *tok, void *arg) {
  VarAttr *attr = arg;
  if (consume_attribute(&tok, tok, "weak")) {
    attr->is_weak = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "hot")) {
    attr->section = ".text.likely";
    return tok;
  }
  if (consume_attribute(&tok, tok, "cold")) {
    attr->section = ".text.unlikely";
    return tok;
  }
  if (consume_attribute(&tok, tok, "section")) {
    tok = skip(tok, '(');
    attr->section = ConsumeStringLiteral(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "noreturn")) {
    attr->is_noreturn = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "always_inline")) {
    attr->is_inline = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "visibility")) {
    tok = skip(tok, '(');
    attr->visibility = ConsumeStringLiteral(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "externally_visible")) {
    attr->is_externally_visible = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "no_instrument_function")) {
    attr->is_no_instrument_function = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "force_align_arg_pointer")) {
    attr->is_force_align_arg_pointer = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "no_caller_saved_registers")) {
    attr->is_no_caller_saved_registers = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "ms_abi")) {
    attr->is_ms_abi = true;
    return tok;
  }
  if (consume_attribute(&tok, tok, "constructor")) {
    attr->is_constructor = true;
    if (CONSUME(&tok, tok, "(")) {
      const_expr(&tok, tok);
      tok = skip(tok, ')');
    }
    return tok;
  }
  if (consume_attribute(&tok, tok, "destructor")) {
    attr->is_destructor = true;
    if (CONSUME(&tok, tok, "(")) {
      const_expr(&tok, tok);
      tok = skip(tok, ')');
    }
    return tok;
  }
  if (consume_attribute(&tok, tok, "aligned")) {
    attr->is_aligned = true;
    if (CONSUME(&tok, tok, "(")) {
      Token *altok = tok;
      attr->align = const_expr(&tok, tok);
      if (popcnt(attr->align) != 1) error_tok(altok, "must be two power");
      tok = skip(tok, ')');
    } else {
      attr->align = 16; /* biggest alignment */
    }
    return tok;
  }
  if (consume_attribute(&tok, tok, "warn_if_not_aligned")) {
    tok = skip(tok, '(');
    const_expr(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "error") ||
      consume_attribute(&tok, tok, "warning")) {
    tok = skip(tok, '(');
    ConsumeStringLiteral(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "noinline") ||
      consume_attribute(&tok, tok, "const") ||
      consume_attribute(&tok, tok, "pure") ||
      consume_attribute(&tok, tok, "dontclone") ||
      consume_attribute(&tok, tok, "may_alias") ||
      consume_attribute(&tok, tok, "warn_unused_result") ||
      consume_attribute(&tok, tok, "flatten") ||
      consume_attribute(&tok, tok, "leaf") ||
      consume_attribute(&tok, tok, "no_reorder") ||
      consume_attribute(&tok, tok, "dontthrow") ||
      consume_attribute(&tok, tok, "optnone") ||
      consume_attribute(&tok, tok, "returns_twice") ||
      consume_attribute(&tok, tok, "nodebug") ||
      consume_attribute(&tok, tok, "artificial") ||
      consume_attribute(&tok, tok, "returns_nonnull") ||
      consume_attribute(&tok, tok, "malloc") ||
      consume_attribute(&tok, tok, "deprecated") ||
      consume_attribute(&tok, tok, "gnu_inline") ||
      consume_attribute(&tok, tok, "used") ||
      consume_attribute(&tok, tok, "unused") ||
      consume_attribute(&tok, tok, "no_icf") ||
      consume_attribute(&tok, tok, "noipa") ||
      consume_attribute(&tok, tok, "noplt") ||
      consume_attribute(&tok, tok, "stack_protect") ||
      consume_attribute(&tok, tok, "no_sanitize_address") ||
      consume_attribute(&tok, tok, "no_sanitize_thread") ||
      consume_attribute(&tok, tok, "no_split_stack") ||
      consume_attribute(&tok, tok, "no_stack_limit") ||
      consume_attribute(&tok, tok, "no_sanitize_undefined") ||
      consume_attribute(&tok, tok, "no_profile_instrument_function")) {
    return tok;
  }
  if (consume_attribute(&tok, tok, "sentinel") ||
      consume_attribute(&tok, tok, "nonnull") ||
      consume_attribute(&tok, tok, "warning") ||
      consume_attribute(&tok, tok, "optimize") ||
      consume_attribute(&tok, tok, "target") ||
      consume_attribute(&tok, tok, "assume_aligned") ||
      consume_attribute(&tok, tok, "visibility") ||
      consume_attribute(&tok, tok, "alloc_size") ||
      consume_attribute(&tok, tok, "alloc_align")) {
    if (CONSUME(&tok, tok, "(")) {
      for (;;) {
        const_expr(&tok, tok);
        if (CONSUME(&tok, tok, ")")) break;
        tok = skip(tok, ',');
      }
    }
    return tok;
  }
  if (consume_attribute(&tok, tok, "format")) {
    tok = skip(tok, '(');
    consume_attribute(&tok, tok, "printf");
    consume_attribute(&tok, tok, "scanf");
    consume_attribute(&tok, tok, "strftime");
    consume_attribute(&tok, tok, "strfmon");
    consume_attribute(&tok, tok, "gnu_printf");
    consume_attribute(&tok, tok, "gnu_scanf");
    consume_attribute(&tok, tok, "gnu_strftime");
    tok = skip(tok, ',');
    const_expr(&tok, tok);
    tok = skip(tok, ',');
    const_expr(&tok, tok);
    return skip(tok, ')');
  }
  if (consume_attribute(&tok, tok, "format_arg")) {
    tok = skip(tok, '(');
    const_expr(&tok, tok);
    return skip(tok, ')');
  }
  error_tok(tok, "unknown function attribute");
}

// declspec = ("void" | "_Bool" | "char" | "short" | "int" | "long"
//             | "typedef" | "static" | "extern" | "inline"
//             | "_Thread_local" | "__thread"
//             | "signed" | "unsigned"
//             | struct-decl | union-decl | typedef-name
//             | enum-specifier | typeof-specifier
//             | "const" | "volatile" | "auto" | "register" | "restrict"
//             | "__restrict" | "__restrict__" | "_Noreturn")+
//
// The order of typenames in a type-specifier doesn't matter. For
// example, `int long static` means the same as `static long int`.
// That can also be written as `static long` because you can omit
// `int` if `long` or `short` are specified. However, something like
// `char int` is not a valid type specifier. We have to accept only a
// limited combinations of the typenames.
//
// In this function, we count the number of occurrences of each typename
// while keeping the "current" type object that the typenames up
// until that point represent. When we reach a non-typename token,
// we returns the current type object.
static Type *declspec(Token **rest, Token *tok, VarAttr *attr) {
  // We use a single integer as counters for all typenames.
  // For example, bits 0 and 1 represents how many times we saw the
  // keyword "void" so far. With this, we can use a switch statement
  // as you can see below.
  enum {
    VOID = 1 << 0,
    BOOL = 1 << 2,
    CHAR = 1 << 4,
    SHORT = 1 << 6,
    INT = 1 << 8,
    LONG = 1 << 10,
    FLOAT = 1 << 12,
    DOUBLE = 1 << 14,
    OTHER = 1 << 16,
    SIGNED = 1 << 17,
    UNSIGNED = 1 << 18,
    INT128 = 1 << 19,
  };
  unsigned char kw;
  Type *ty = copy_type(ty_long);  // [jart] use long as implicit type
  int counter = 0;
  bool is_const = false;
  bool is_atomic = false;
  while (is_typename(tok)) {
    if ((kw = GetKw(tok->loc, tok->len))) {
      // Handle storage class specifiers.
      if (kw == KW_TYPEDEF || kw == KW_STATIC || kw == KW_EXTERN ||
          kw == KW_INLINE || kw == KW__THREAD_LOCAL) {
        if (!attr)
          error_tok(tok,
                    "storage class specifier is not allowed in this context");
        if (kw == KW_TYPEDEF) {
          attr->is_typedef = true;
        } else if (kw == KW_STATIC) {
          attr->is_static = true;
        } else if (kw == KW_EXTERN) {
          attr->is_extern = true;
        } else if (kw == KW_INLINE) {
          attr->is_inline = true;
        } else {
          attr->is_tls = true;
        }
        if (attr->is_typedef &&
            attr->is_static + attr->is_extern + attr->is_inline + attr->is_tls >
                1) {
          error_tok(tok, "typedef may not be used together with static,"
                         " extern, inline, __thread or _Thread_local");
        }
        tok = tok->next;
        goto Continue;
      }
      if (kw == KW__NORETURN) {
        if (attr) attr->is_noreturn = true;
        tok = tok->next;
        goto Continue;
      }
      if (kw == KW_CONST) {
        is_const = true;
        tok = tok->next;
        goto Continue;
      }
      // These keywords are recognized but ignored.
      if (kw == KW_VOLATILE || kw == KW_AUTO || kw == KW_REGISTER ||
          kw == KW_RESTRICT) {
        tok = tok->next;
        goto Continue;
      }
      if (kw == KW__ATOMIC) {
        tok = tok->next;
        if (EQUAL(tok, "(")) {
          ty = typename(&tok, tok->next);
          tok = skip(tok, ')');
        }
        is_atomic = true;
        goto Continue;
      }
      if (kw == KW__ALIGNAS) {
        if (!attr) error_tok(tok, "_Alignas is not allowed in this context");
        tok = skip(tok->next, '(');
        if (is_typename(tok)) {
          attr->align = typename(&tok, tok)->align;
        } else {
          Token *altok = tok;
          attr->align = const_expr(&tok, tok);
          if (popcnt(ty->align) != 1) {
            error_tok(altok, "_Alignas needs two power");
          }
        }
        tok = skip(tok, ')');
        goto Continue;
      }
    }
    // Handle user-defined types.
    Type *ty2 = find_typedef(tok);
    if (ty2 || kw == KW_STRUCT || kw == KW_UNION || kw == KW_ENUM ||
        kw == KW_TYPEOF) {
      if (counter) break;
      if (kw == KW_STRUCT) {
        ty = struct_decl(&tok, tok->next);
      } else if (kw == KW_UNION) {
        ty = union_decl(&tok, tok->next);
      } else if (kw == KW_ENUM) {
        ty = enum_specifier(&tok, tok->next);
      } else if (kw == KW_TYPEOF) {
        ty = typeof_specifier(&tok, tok->next);
      } else {
        ty = ty2;
        tok = tok->next;
      }
      counter += OTHER;
      goto Continue;
    }
    // Handle built-in types.
    if (kw == KW_VOID) {
      counter += VOID;
    } else if (kw == KW__BOOL) {
      counter += BOOL;
    } else if (kw == KW_CHAR) {
      counter += CHAR;
    } else if (kw == KW_SHORT) {
      counter += SHORT;
    } else if (kw == KW_INT) {
      counter += INT;
    } else if (kw == KW_LONG) {
      counter += LONG;
    } else if (kw == KW___INT128) {
      counter += INT128;
    } else if (kw == KW_FLOAT) {
      counter += FLOAT;
    } else if (kw == KW_DOUBLE) {
      counter += DOUBLE;
    } else if (kw == KW_SIGNED) {
      counter |= SIGNED;
    } else if (kw == KW_UNSIGNED) {
      counter |= UNSIGNED;
    } else {
      UNREACHABLE();
    }
    switch (counter) {
      case VOID:
        ty = copy_type(ty_void);
        break;
      case BOOL:
        ty = copy_type(ty_bool);
        break;
      case CHAR:
      case SIGNED + CHAR:
        ty = copy_type(ty_char);
        break;
      case UNSIGNED + CHAR:
        ty = copy_type(ty_uchar);
        break;
      case SHORT:
      case SHORT + INT:
      case SIGNED + SHORT:
      case SIGNED + SHORT + INT:
        ty = copy_type(ty_short);
        break;
      case UNSIGNED + SHORT:
      case UNSIGNED + SHORT + INT:
        ty = copy_type(ty_ushort);
        break;
      case INT:
      case SIGNED:
      case SIGNED + INT:
        ty = copy_type(ty_int);
        break;
      case UNSIGNED:
      case UNSIGNED + INT:
        ty = copy_type(ty_uint);
        break;
      case LONG:
      case LONG + INT:
      case LONG + LONG:
      case LONG + LONG + INT:
      case SIGNED + LONG:
      case SIGNED + LONG + INT:
      case SIGNED + LONG + LONG:
      case SIGNED + LONG + LONG + INT:
        ty = copy_type(ty_long);
        break;
      case UNSIGNED + LONG:
      case UNSIGNED + LONG + INT:
      case UNSIGNED + LONG + LONG:
      case UNSIGNED + LONG + LONG + INT:
        ty = copy_type(ty_ulong);
        break;
      case INT128:
      case SIGNED + INT128:
        ty = copy_type(ty_int128);
        break;
      case UNSIGNED + INT128:
        ty = copy_type(ty_uint128);
        break;
      case FLOAT:
        ty = copy_type(ty_float);
        break;
      case DOUBLE:
        ty = copy_type(ty_double);
        break;
      case LONG + DOUBLE:
        ty = copy_type(ty_ldouble);
        break;
      default:
        error_tok(tok, "invalid type");
    }
    tok = tok->next;
  Continue:
    if (attr && attr->is_typedef) {
      tok = attribute_list(tok, ty, type_attributes);
    } else if (attr) {
      tok = attribute_list(tok, attr, thing_attributes);
    }
  }
  if (is_atomic) {
    ty = copy_type(ty);
    ty->is_atomic = true;
  }
  if (is_const) {
    ty = copy_type(ty);
    ty->is_const = true;
  }
  *rest = tok;
  return ty;
}

static Token *static_assertion(Token *tok) {
  char *msg;
  Token *start = tok;
  tok = skip(tok->next, '(');
  int64_t cond = const_expr(&tok, tok);
  if (CONSUME(&tok, tok, ",")) {
    msg = ConsumeStringLiteral(&tok, tok);
  } else {
    msg = "assertion failed";
  }
  tok = skip(tok, ')');
  tok = skip(tok, ';');
  if (!cond) {
    error_tok(start, "%s", msg);
  }
  return tok;
}

// func-params = ("void" | param ("," param)* ("," "...")?)? ")"
// param       = declspec declarator
static Type *func_params(Token **rest, Token *tok, Type *ty) {
  if (EQUAL(tok, "void") && EQUAL(tok->next, ")")) {
    *rest = tok->next->next;
    return func_type(ty);
  }
  Type head = {};
  Type *cur = &head;
  bool is_variadic = false;
  enter_scope();
  while (!EQUAL(tok, ")")) {
    if (cur != &head) tok = skip(tok, ',');
    if (EQUAL(tok, "...")) {
      is_variadic = true;
      tok = tok->next;
      skip(tok, ')');
      break;
    }
    Type *ty2 = declspec(&tok, tok, NULL);
    ty2 = declarator(&tok, tok, ty2);
    Token *name = ty2->name;
    if (name) {
      new_lvar(strndup(name->loc, name->len), ty2);
    }
    if (ty2->kind == TY_ARRAY) {
      // "array of T" decays to "pointer to T" only in the parameter
      // context. For example, *argv[] is converted to **argv here.
      Type *ty3 = ty2;
      ty3 = pointer_to(ty2->base);
      ty3->name = name;
      ty3->array_len = ty2->array_len;
      ty3->is_static = ty2->is_static;
      ty3->is_restrict = ty2->is_restrict;
      ty2 = ty3;
    } else if (ty2->kind == TY_FUNC) {
      // Likewise, a function is converted to a pointer to a function
      // only in the parameter context.
      ty2 = pointer_to(ty2);
      ty2->name = name;
    }
    cur = cur->next = copy_type(ty2);
  }
  leave_scope();
  if (cur == &head) is_variadic = true;
  ty = func_type(ty);
  ty->params = head.next;
  ty->is_variadic = is_variadic;
  *rest = tok->next;
  return ty;
}

// array-dimensions = ("static" | "restrict")* const-expr? "]" type-suffix
static Type *array_dimensions(Token **rest, Token *tok, Type *ty) {
  Node *expr;
  bool is_static, is_restrict;
  is_static = false;
  is_restrict = false;
  for (;; tok = tok->next) {
    if (EQUAL(tok, "static")) {
      is_static = true;
    } else if (EQUAL(tok, "restrict")) {
      is_restrict = true;
    } else {
      break;
    }
  }
  if (EQUAL(tok, "]")) {
    ty = type_suffix(rest, tok->next, ty);
    ty = array_of(ty, -1);
  } else {
    expr = conditional(&tok, tok);
    tok = skip(tok, ']');
    ty = type_suffix(rest, tok, ty);
    if (ty->kind == TY_VLA || !is_const_expr(expr)) return vla_of(ty, expr);
    ty = array_of(ty, eval(expr));
  }
  ty->is_static = is_static;
  ty->is_restrict = is_restrict;
  return ty;
}

// type-suffix = "(" func-params
//             | "[" array-dimensions
//             | ε
static Type *type_suffix(Token **rest, Token *tok, Type *ty) {
  if (EQUAL(tok, "(")) return func_params(rest, tok->next, ty);
  if (EQUAL(tok, "[")) return array_dimensions(rest, tok->next, ty);
  *rest = tok;
  return ty;
}

// pointers = ("*" ("const" | "volatile" | "restrict" | attribute)*)*
static Type *pointers(Token **rest, Token *tok, Type *ty) {
  while (CONSUME(&tok, tok, "*")) {
    ty = pointer_to(ty);
    for (;;) {
      tok = attribute_list(tok, ty, type_attributes);
      if (EQUAL(tok, "const")) {
        ty->is_const = true;
        tok = tok->next;
      } else if (EQUAL(tok, "volatile")) {
        ty->is_volatile = true;
        tok = tok->next;
      } else if (EQUAL(tok, "restrict") || EQUAL(tok, "__restrict") ||
                 EQUAL(tok, "__restrict__")) {
        ty->is_restrict = true;
        tok = tok->next;
      } else {
        break;
      }
    }
  }
  *rest = tok;
  return ty;
}

// declarator = pointers ("(" ident ")" | "(" declarator ")" | ident)
// type-suffix
static Type *declarator(Token **rest, Token *tok, Type *ty) {
  ty = pointers(&tok, tok, ty);
  if (EQUAL(tok, "(")) {
    Token *start = tok;
    Type dummy = {};
    declarator(&tok, start->next, &dummy);
    tok = skip(tok, ')');
    ty = type_suffix(rest, tok, ty);
    ty = declarator(&tok, start->next, ty);
    return ty;
  }
  Token *name = NULL;
  Token *name_pos = tok;
  if (tok->kind == TK_IDENT) {
    name = tok;
    tok = tok->next;
  }
  ty = type_suffix(rest, tok, ty);
  ty->name = name;
  ty->name_pos = name_pos;
  return ty;
}

// abstract-declarator = pointers ("(" abstract-declarator ")")? type-suffix
static Type *abstract_declarator(Token **rest, Token *tok, Type *ty) {
  ty = pointers(&tok, tok, ty);
  if (EQUAL(tok, "(")) {
    Token *start = tok;
    Type dummy = {};
    abstract_declarator(&tok, start->next, &dummy);
    tok = skip(tok, ')');
    ty = type_suffix(rest, tok, ty);
    return abstract_declarator(&tok, start->next, ty);
  }
  return type_suffix(rest, tok, ty);
}

// type-name = declspec abstract-declarator
static Type *typename(Token **rest, Token *tok) {
  Type *ty = declspec(&tok, tok, NULL);
  return abstract_declarator(rest, tok, ty);
}

static bool is_end(Token *tok) {
  return EQUAL(tok, "}") || (EQUAL(tok, ",") && EQUAL(tok->next, "}"));
}

static bool consume_end(Token **rest, Token *tok) {
  if (EQUAL(tok, "}")) {
    *rest = tok->next;
    return true;
  }
  if (EQUAL(tok, ",") && EQUAL(tok->next, "}")) {
    *rest = tok->next->next;
    return true;
  }
  return false;
}

// enum-specifier = ident? "{" enum-list? "}"
//                | ident ("{" enum-list? "}")?
//
// enum-list      = ident ("=" num)? ("," ident ("=" num)?)* ","?
static Type *enum_specifier(Token **rest, Token *tok) {
  Type *ty = enum_type();
  // Read a struct tag.
  Token *tag = NULL;
  if (tok->kind == TK_IDENT) {
    tag = tok;
    tok = tok->next;
  }
  if (tag && !EQUAL(tok, "{")) {
    Type *ty = find_tag(tag);
    if (!ty) error_tok(tag, "unknown enum type");
    if (ty->kind != TY_ENUM) error_tok(tag, "not an enum tag");
    *rest = tok;
    return ty;
  }
  ty->name = tag;
  tok = skip(tok, '{');
  // Read an enum-list.
  int i = 0;
  int val = 0;
  while (!consume_end(rest, tok)) {
    if (i++ > 0) tok = skip(tok, ',');
    if (tok->kind == TK_JAVADOWN) {
      current_javadown = tok;
      tok = tok->next;
    }
    char *name = get_ident(tok);
    tok = tok->next;
    if (EQUAL(tok, "=")) val = const_expr(&tok, tok->next);
    VarScope *sc = push_scope(name);
    sc->enum_ty = ty;
    sc->enum_val = val++;
  }
  if (tag) push_tag_scope(tag, ty);
  return ty;
}

// typeof-specifier = "(" (expr | typename) ")"
static Type *typeof_specifier(Token **rest, Token *tok) {
  tok = skip(tok, '(');
  Type *ty;
  if (is_typename(tok)) {
    ty = typename(&tok, tok);
  } else {
    Node *node = expr(&tok, tok);
    add_type(node);
    ty = node->ty;
  }
  *rest = skip(tok, ')');
  return ty;
}

// Generate code for computing a VLA size.
static Node *compute_vla_size(Type *ty, Token *tok) {
  Node *node = new_node(ND_NULL_EXPR, tok);
  if (ty->base) {
    node = new_binary(ND_COMMA, node, compute_vla_size(ty->base, tok), tok);
  }
  if (ty->kind != TY_VLA) return node;
  Node *base_sz;
  if (ty->base->kind == TY_VLA) {
    base_sz = new_var_node(ty->base->vla_size, tok);
  } else {
    base_sz = new_num(ty->base->size, tok);
  }
  ty->vla_size = new_lvar("", ty_ulong);
  Node *expr = new_binary(ND_ASSIGN, new_var_node(ty->vla_size, tok),
                          new_binary(ND_MUL, ty->vla_len, base_sz, tok), tok);
  return new_binary(ND_COMMA, node, expr, tok);
}

static Node *new_alloca(Node *sz) {
  Node *node;
  node = new_unary(ND_FUNCALL, new_var_node(builtin_alloca, sz->tok), sz->tok);
  node->func_ty = builtin_alloca->ty;
  node->ty = builtin_alloca->ty->return_ty;
  node->args = sz;
  add_type(sz);
  return node;
}

// declaration = declspec (declarator ("=" expr)?
//                         ("," declarator ("=" expr)?)*)? ";"
static Node *declaration(Token **rest, Token *tok, Type *basety,
                         VarAttr *attr) {
  Node head = {};
  Node *cur = &head;
  int i = 0;
  while (!EQUAL(tok, ";")) {
    if (i++ > 0) tok = skip(tok, ',');
    Type *ty = declarator(&tok, tok, basety);
    if (ty->kind == TY_VOID) error_tok(tok, "variable declared void");
    if (!ty->name) error_tok(ty->name_pos, "variable name omitted");
    if (attr && attr->is_static) {
      // static local variable
      Obj *var = new_anon_gvar(ty);
      push_scope(get_ident(ty->name))->var = var;
      if (EQUAL(tok, "=")) gvar_initializer(&tok, tok->next, var);
      continue;
    }
    // Generate code for computing a VLA size. We need to do this
    // even if ty is not VLA because ty may be a pointer to VLA
    // (e.g. int (*foo)[n][m] where n and m are variables.)
    cur = cur->next = new_unary(ND_EXPR_STMT, compute_vla_size(ty, tok), tok);
    tok = attribute_list(tok, attr, thing_attributes);
    if (ty->kind == TY_VLA) {
      if (EQUAL(tok, "="))
        error_tok(tok, "variable-sized object may not be initialized");
      // Variable length arrays (VLAs) are translated to alloca() calls.
      // For example, `int x[n+2]` is translated to `tmp = n + 2,
      // x = alloca(tmp)`.
      Obj *var = new_lvar(get_ident(ty->name), ty);
      Token *tok = ty->name;
      Node *expr = new_binary(ND_ASSIGN, new_vla_ptr(var, tok),
                              new_alloca(new_var_node(ty->vla_size, tok)), tok);
      cur = cur->next = new_unary(ND_EXPR_STMT, expr, tok);
      continue;
    }
    Obj *var = new_lvar(get_ident(ty->name), ty);
    if (attr && attr->align) var->align = attr->align;
    if (EQUAL(tok, "=")) {
      Node *expr = lvar_initializer(&tok, tok->next, var);
      cur = cur->next = new_unary(ND_EXPR_STMT, expr, tok);
    }
    if (var->ty->size < 0) error_tok(ty->name, "variable has incomplete type");
    if (var->ty->kind == TY_VOID) error_tok(ty->name, "variable declared void");
  }
  Node *node = new_node(ND_BLOCK, tok);
  node->body = head.next;
  *rest = tok->next;
  return node;
}

static Token *skip_excess_element(Token *tok) {
  if (EQUAL(tok, "{")) {
    tok = skip_excess_element(tok->next);
    return skip(tok, '}');
  }
  assign(&tok, tok);
  return tok;
}

// string-initializer = string-literal
static void string_initializer(Token **rest, Token *tok, Initializer *init) {
  if (init->is_flexible) {
    *init =
        *new_initializer(array_of(init->ty->base, tok->ty->array_len), false);
  }
  int len = MIN(init->ty->array_len, tok->ty->array_len);
  switch (init->ty->base->size) {
    case 1: {
      char *str = tok->str;
      for (int i = 0; i < len; i++)
        init->children[i]->expr = new_num(str[i], tok);
      break;
    }
    case 2: {
      uint16_t *str = (uint16_t *)tok->str;
      for (int i = 0; i < len; i++)
        init->children[i]->expr = new_num(str[i], tok);
      break;
    }
    case 4: {
      uint32_t *str = (uint32_t *)tok->str;
      for (int i = 0; i < len; i++)
        init->children[i]->expr = new_num(str[i], tok);
      break;
    }
    default:
      UNREACHABLE();
  }
  *rest = tok->next;
}

// array-designator = "[" const-expr "]"
//
// C99 added the designated initializer to the language, which allows
// programmers to move the "cursor" of an initializer to any element.
// The syntax looks like this:
//
//   int x[10] = { 1, 2, [5]=3, 4, 5, 6, 7 };
//
// `[5]` moves the cursor to the 5th element, so the 5th element of x
// is set to 3. Initialization then continues forward in order, so
// 6th, 7th, 8th and 9th elements are initialized with 4, 5, 6 and 7,
// respectively. Unspecified elements (in this case, 3rd and 4th
// elements) are initialized with zero.
//
// Nesting is allowed, so the following initializer is valid:
//
//   int x[5][10] = { [5][8]=1, 2, 3 };
//
// It sets x[5][8], x[5][9] and x[6][0] to 1, 2 and 3, respectively.
//
// Use `.fieldname` to move the cursor for a struct initializer. E.g.
//
//   struct { int a, b, c; } x = { .c=5 };
//
// The above initializer sets x.c to 5.
static void array_designator(Token **rest, Token *tok, Type *ty, int *begin,
                             int *end) {
  *begin = const_expr(&tok, tok->next);
  if (*begin >= ty->array_len)
    error_tok(tok, "array designator index exceeds array bounds");
  if (EQUAL(tok, "...")) {
    *end = const_expr(&tok, tok->next);
    if (*end >= ty->array_len)
      error_tok(tok, "array designator index exceeds array bounds");
    if (*end < *begin)
      error_tok(tok, "array designator range [%d, %d] is empty", *begin, *end);
  } else {
    *end = *begin;
  }
  *rest = skip(tok, ']');
}

// struct-designator = "." ident
static Member *struct_designator(Token **rest, Token *tok, Type *ty) {
  Token *start = tok;
  tok = skip(tok, '.');
  if (tok->kind == TK_JAVADOWN) {
    current_javadown = tok;
    tok = tok->next;
  }
  if (tok->kind != TK_IDENT) error_tok(tok, "expected a field designator");
  for (Member *mem = ty->members; mem; mem = mem->next) {
    // Anonymous struct member
    if ((mem->ty->kind == TY_STRUCT || mem->ty->kind == TY_UNION) &&
        !mem->name) {
      if (get_struct_member(mem->ty, tok)) {
        *rest = start;
        return mem;
      }
      continue;
    }
    // Regular struct member
    if (mem->name->len == tok->len &&
        !strncmp(mem->name->loc, tok->loc, tok->len)) {
      *rest = tok->next;
      return mem;
    }
  }
  error_tok(tok, "struct has no such member");
}

// designation = ("[" const-expr "]" | "." ident)* "="? initializer
static void designation(Token **rest, Token *tok, Initializer *init) {
  if (EQUAL(tok, "[")) {
    if (init->ty->kind != TY_ARRAY)
      error_tok(tok, "array index in non-array initializer");
    int begin, end;
    array_designator(&tok, tok, init->ty, &begin, &end);
    Token *tok2;
    for (int i = begin; i <= end; i++)
      designation(&tok2, tok, init->children[i]);
    array_initializer2(rest, tok2, init, begin + 1);
    return;
  }
  if (EQUAL(tok, ".") && init->ty->kind == TY_STRUCT) {
    Member *mem = struct_designator(&tok, tok, init->ty);
    designation(&tok, tok, init->children[mem->idx]);
    init->expr = NULL;
    struct_initializer2(rest, tok, init, mem->next);
    return;
  }
  if (EQUAL(tok, ".") && init->ty->kind == TY_UNION) {
    Member *mem = struct_designator(&tok, tok, init->ty);
    init->mem = mem;
    designation(rest, tok, init->children[mem->idx]);
    return;
  }
  if (EQUAL(tok, ".")) {
    error_tok(tok, "field name not in struct or union initializer");
  }
  if (EQUAL(tok, "=")) tok = tok->next;
  initializer2(rest, tok, init);
}

// An array length can be omitted if an array has an initializer
// (e.g. `int x[] = {1,2,3}`). If it's omitted, count the number
// of initializer elements.
static int count_array_init_elements(Token *tok, Type *ty) {
  bool first = true;
  Initializer *dummy = new_initializer(ty->base, true);
  int i = 0, max = 0;
  while (!consume_end(&tok, tok)) {
    if (!first) tok = skip(tok, ',');
    first = false;
    if (EQUAL(tok, "[")) {
      i = const_expr(&tok, tok->next);
      if (EQUAL(tok, "...")) i = const_expr(&tok, tok->next);
      tok = skip(tok, ']');
      designation(&tok, tok, dummy);
    } else {
      initializer2(&tok, tok, dummy);
    }
    i++;
    max = MAX(max, i);
  }
  return max;
}

// array-initializer1 = "{" initializer ("," initializer)* ","? "}"
static void array_initializer1(Token **rest, Token *tok, Initializer *init) {
  tok = skip(tok, '{');
  if (init->is_flexible) {
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);
  }
  bool first = true;
  if (init->is_flexible) {
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);
  }
  for (int i = 0; !consume_end(rest, tok); i++) {
    if (!first) tok = skip(tok, ',');
    first = false;
    if (EQUAL(tok, "[")) {
      int begin, end;
      array_designator(&tok, tok, init->ty, &begin, &end);
      Token *tok2;
      for (int j = begin; j <= end; j++)
        designation(&tok2, tok, init->children[j]);
      tok = tok2;
      i = end;
      continue;
    }
    if (i < init->ty->array_len) {
      initializer2(&tok, tok, init->children[i]);
    } else {
      tok = skip_excess_element(tok);
    }
  }
}

// array-initializer2 = initializer (',' initializer)*
static void array_initializer2(Token **rest, Token *tok, Initializer *init,
                               int i) {
  if (init->is_flexible) {
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);
  }
  for (; i < init->ty->array_len && !is_end(tok); i++) {
    Token *start = tok;
    if (i > 0) tok = skip(tok, ',');
    if (EQUAL(tok, "[") || EQUAL(tok, ".")) {
      *rest = start;
      return;
    }
    initializer2(&tok, tok, init->children[i]);
  }
  *rest = tok;
}

// struct-initializer1 = "{" initializer ("," initializer)* ","? "}"
static void struct_initializer1(Token **rest, Token *tok, Initializer *init) {
  tok = skip(tok, '{');
  Member *mem = init->ty->members;
  bool first = true;
  while (!consume_end(rest, tok)) {
    if (!first) tok = skip(tok, ',');
    first = false;
    if (EQUAL(tok, ".")) {
      mem = struct_designator(&tok, tok, init->ty);
      designation(&tok, tok, init->children[mem->idx]);
      mem = mem->next;
      continue;
    }
    if (mem) {
      initializer2(&tok, tok, init->children[mem->idx]);
      mem = mem->next;
    } else {
      tok = skip_excess_element(tok);
    }
  }
}

// struct-initializer2 = initializer ("," initializer)*
static void struct_initializer2(Token **rest, Token *tok, Initializer *init,
                                Member *mem) {
  bool first = true;
  for (; mem && !is_end(tok); mem = mem->next) {
    Token *start = tok;
    if (!first) tok = skip(tok, ',');
    first = false;
    if (EQUAL(tok, "[") || EQUAL(tok, ".")) {
      *rest = start;
      return;
    }
    initializer2(&tok, tok, init->children[mem->idx]);
  }
  *rest = tok;
}

static void union_initializer(Token **rest, Token *tok, Initializer *init) {
  // Unlike structs, union initializers take only one initializer,
  // and that initializes the first union member by default.
  // You can initialize other member using a designated initializer.
  if (EQUAL(tok, "{") && EQUAL(tok->next, ".")) {
    Member *mem = struct_designator(&tok, tok->next, init->ty);
    init->mem = mem;
    designation(&tok, tok, init->children[mem->idx]);
    *rest = skip(tok, '}');
    return;
  }
  init->mem = init->ty->members;
  if (EQUAL(tok, "{")) {
    initializer2(&tok, tok->next, init->children[0]);
    CONSUME(&tok, tok, ",");
    *rest = skip(tok, '}');
  } else {
    initializer2(rest, tok, init->children[0]);
  }
}

// initializer = string-initializer | array-initializer
//             | struct-initializer | union-initializer
//             | assign
static void initializer2(Token **rest, Token *tok, Initializer *init) {
  if (EQUAL(tok, "(") && init->ty->kind == TY_ARRAY &&
      tok->next->kind == TK_STR) {
    /* XXX: Kludge so typeof("str") s = ("str"); macros work. */
    initializer2(rest, tok->next, init);
    *rest = skip(*rest, ')');
    return;
  }
  if (init->ty->kind == TY_ARRAY && tok->kind == TK_STR) {
    string_initializer(rest, tok, init);
    return;
  }
  if (init->ty->kind == TY_ARRAY) {
    if (EQUAL(tok, "{")) {
      array_initializer1(rest, tok, init);
    } else {
      array_initializer2(rest, tok, init, 0);
    }
    return;
  }
  if (init->ty->kind == TY_STRUCT) {
    if (EQUAL(tok, "{")) {
      struct_initializer1(rest, tok, init);
      return;
    }
    // A struct can be initialized with another struct. E.g.
    // `struct T x = y;` where y is a variable of type `struct T`.
    // Handle that case first.
    Node *expr = assign(rest, tok);
    add_type(expr);
    if (expr->ty->kind == TY_STRUCT) {
      init->expr = expr;
      return;
    }
    struct_initializer2(rest, tok, init, init->ty->members);
    return;
  }
  if (init->ty->kind == TY_UNION) {
    union_initializer(rest, tok, init);
    return;
  }
  if (EQUAL(tok, "{")) {
    // An initializer for a scalar variable can be surrounded by
    // braces. E.g. `int x = {3};`. Handle that case.
    initializer2(&tok, tok->next, init);
    *rest = skip(tok, '}');
    return;
  }
  init->expr = assign(rest, tok);
}

static Type *copy_struct_type(Type *ty) {
  ty = copy_type(ty);
  Member head = {};
  Member *cur = &head;
  for (Member *mem = ty->members; mem; mem = mem->next) {
    Member *m = calloc(1, sizeof(Member));
    *m = *mem;
    cur = cur->next = m;
  }
  ty->members = head.next;
  return ty;
}

static Initializer *initializer(Token **rest, Token *tok, Type *ty,
                                Type **new_ty) {
  Initializer *init = new_initializer(ty, true);
  initializer2(rest, tok, init);
  if ((ty->kind == TY_STRUCT || ty->kind == TY_UNION) && ty->is_flexible) {
    ty = copy_struct_type(ty);
    Member *mem = ty->members;
    while (mem->next) mem = mem->next;
    mem->ty = init->children[mem->idx]->ty;
    ty->size += mem->ty->size;
    *new_ty = ty;
    return init;
  }
  *new_ty = init->ty;
  return init;
}

static Node *init_desg_expr(InitDesg *desg, Token *tok) {
  if (desg->var) return new_var_node(desg->var, tok);
  if (desg->member) {
    Node *node = new_unary(ND_MEMBER, init_desg_expr(desg->next, tok), tok);
    node->member = desg->member;
    return node;
  }
  Node *lhs = init_desg_expr(desg->next, tok);
  Node *rhs = new_num(desg->idx, tok);
  return new_unary(ND_DEREF, new_add(lhs, rhs, tok), tok);
}

static Node *create_lvar_init(Initializer *init, Type *ty, InitDesg *desg,
                              Token *tok) {
  if (ty->kind == TY_ARRAY) {
    Node *node = new_node(ND_NULL_EXPR, tok);
    for (int i = 0; i < ty->array_len; i++) {
      InitDesg desg2 = {desg, i};
      Node *rhs = create_lvar_init(init->children[i], ty->base, &desg2, tok);
      node = new_binary(ND_COMMA, node, rhs, tok);
    }
    return node;
  }
  if (ty->kind == TY_STRUCT && !init->expr) {
    Node *node = new_node(ND_NULL_EXPR, tok);
    for (Member *mem = ty->members; mem; mem = mem->next) {
      InitDesg desg2 = {desg, 0, mem};
      Node *rhs =
          create_lvar_init(init->children[mem->idx], mem->ty, &desg2, tok);
      node = new_binary(ND_COMMA, node, rhs, tok);
    }
    return node;
  }
  if (ty->kind == TY_UNION) {
    Member *mem = init->mem ? init->mem : ty->members;
    InitDesg desg2 = {desg, 0, mem};
    return create_lvar_init(init->children[mem->idx], mem->ty, &desg2, tok);
  }
  if (!init->expr) return new_node(ND_NULL_EXPR, tok);
  Node *lhs = init_desg_expr(desg, tok);
  return new_binary(ND_ASSIGN, lhs, init->expr, tok);
}

// A variable definition with an initializer is a shorthand notation
// for a variable definition followed by assignments. This function
// generates assignment expressions for an initializer. For example,
// `int x[2][2] = {{6, 7}, {8, 9}}` is converted to the following
// expressions:
//
//   x[0][0] = 6;
//   x[0][1] = 7;
//   x[1][0] = 8;
//   x[1][1] = 9;
//
static Node *lvar_initializer(Token **rest, Token *tok, Obj *var) {
  Initializer *init = initializer(rest, tok, var->ty, &var->ty);
  InitDesg desg = {NULL, 0, NULL, var};
  // If a partial initializer list is given, the standard requires
  // that unspecified elements are set to 0. Here, we simply
  // zero-initialize the entire memory region of a variable before
  // initializing it with user-supplied values.
  Node *lhs = new_node(ND_MEMZERO, tok);
  lhs->var = var;
  Node *rhs = create_lvar_init(init, var->ty, &desg, tok);
  return new_binary(ND_COMMA, lhs, rhs, tok);
}

static uint64_t read_buf(char *buf, int sz) {
  if (sz == 1) return *buf;
  if (sz == 2) return *(uint16_t *)buf;
  if (sz == 4) return *(uint32_t *)buf;
  if (sz == 8) return *(uint64_t *)buf;
  UNREACHABLE();
}

static void write_buf(char *buf, uint64_t val, int sz) {
  if (sz == 1) {
    *buf = val;
  } else if (sz == 2) {
    *(uint16_t *)buf = val;
  } else if (sz == 4) {
    *(uint32_t *)buf = val;
  } else if (sz == 8) {
    *(uint64_t *)buf = val;
  } else {
    UNREACHABLE();
  }
}

static Relocation *write_gvar_data(Relocation *cur, Initializer *init, Type *ty,
                                   char *buf, int offset) {
  if (ty->kind == TY_ARRAY) {
    int sz = ty->base->size;
    for (int i = 0; i < ty->array_len; i++)
      cur = write_gvar_data(cur, init->children[i], ty->base, buf,
                            offset + sz * i);
    return cur;
  }
  if (ty->kind == TY_STRUCT) {
    for (Member *mem = ty->members; mem; mem = mem->next) {
      if (mem->is_bitfield) {
        Node *expr = init->children[mem->idx]->expr;
        if (!expr) break;
        char *loc = buf + offset + mem->offset;
        uint64_t oldval = read_buf(loc, mem->ty->size);
        uint64_t newval = eval(expr);
        uint64_t mask = (1L << mem->bit_width) - 1;
        uint64_t combined = oldval | ((newval & mask) << mem->bit_offset);
        write_buf(loc, combined, mem->ty->size);
      } else {
        cur = write_gvar_data(cur, init->children[mem->idx], mem->ty, buf,
                              offset + mem->offset);
      }
    }
    return cur;
  }
  if (ty->kind == TY_UNION) {
    if (!init->mem) return cur;
    return write_gvar_data(cur, init->children[init->mem->idx], init->mem->ty,
                           buf, offset);
  }
  if (!init->expr) return cur;
  if (ty->kind == TY_FLOAT) {
    *(float *)(buf + offset) = eval_double(init->expr);
    return cur;
  }
  if (ty->kind == TY_DOUBLE) {
    *(double *)(buf + offset) = eval_double(init->expr);
    return cur;
  }
  char **label = NULL;
  uint64_t val = eval2(init->expr, &label);
  if (!label) {
    write_buf(buf + offset, val, ty->size);
    return cur;
  }
  Relocation *rel = calloc(1, sizeof(Relocation));
  rel->offset = offset;
  rel->label = label;
  rel->addend = val;
  cur->next = rel;
  return cur->next;
}

// Initializers for global variables are evaluated at compile-time and
// embedded to .data section. This function serializes Initializer
// objects to a flat byte array. It is a compile error if an
// initializer list contains a non-constant expression.
static void gvar_initializer(Token **rest, Token *tok, Obj *var) {
  Initializer *init = initializer(rest, tok, var->ty, &var->ty);
  Relocation head = {};
  char *buf = calloc(1, var->ty->size);
  write_gvar_data(&head, init, var->ty, buf, 0);
  var->init_data = buf;
  var->rel = head.next;
}

// Returns true if a given token represents a type.
static bool is_typename(Token *tok) {
  unsigned char kw;
  kw = GetKw(tok->loc, tok->len);
  return (kw && !(kw & -32)) || find_typedef(tok);
}

static bool is_const_expr_true(Node *node) {
  if (is_flonum(node->ty)) {
    return !!eval_double(node);
  } else {
    return eval(node);
  }
}

// stmt = "return" expr? ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "switch" "(" expr ")" stmt
//      | "case" const-expr ("..." const-expr)? ":" stmt
//      | "default" ":" stmt
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "do" stmt "while" "(" expr ")" ";"
//      | "asm" asm-stmt
//      | "goto" (ident | "*" expr) ";"
//      | "break" ";"
//      | "continue" ";"
//      | ident ":" stmt
//      | "{" compound-stmt
//      | expr-stmt
static Node *stmt(Token **rest, Token *tok) {
  if (EQUAL(tok, "return")) {
    Node *node = new_node(ND_RETURN, tok);
    if (CONSUME(rest, tok->next, ";")) return node;
    Node *exp = expr(&tok, tok->next);
    *rest = skip(tok, ';');
    add_type(exp);
    Type *ty = current_fn->ty->return_ty;
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)
      exp = new_cast(exp, current_fn->ty->return_ty);
    node->lhs = exp;
    return node;
  }
  if (EQUAL(tok, "if")) {
    Node *node = new_node(ND_IF, tok);
    tok = skip(tok->next, '(');
    node->cond = expr(&tok, tok);
    tok = skip(tok, ')');
    node->then = stmt(&tok, tok);
    if (EQUAL(tok, "else")) node->els = stmt(&tok, tok->next);
    *rest = tok;
    if (is_const_expr(node->cond)) {
      if (is_const_expr_true(node->cond)) { /* DCE */
        return node->then;
      } else if (node->els) {
        return node->els;
      } else {
        return new_node(ND_BLOCK, node->tok);
      }
    }
    return node;
  }
  if (EQUAL(tok, "_Static_assert")) {
    Token *start = tok;
    *rest = static_assertion(tok);
    return new_node(ND_BLOCK, start);
  }
  if (EQUAL(tok, "switch")) {
    Node *node = new_node(ND_SWITCH, tok);
    tok = skip(tok->next, '(');
    node->cond = expr(&tok, tok);
    tok = skip(tok, ')');
    Node *sw = current_switch;
    current_switch = node;
    char *brk = brk_label;
    brk_label = node->brk_label = new_unique_name();
    node->then = stmt(rest, tok);
    current_switch = sw;
    brk_label = brk;
    return node;
  }
  if (EQUAL(tok, "case")) {
    if (!current_switch) error_tok(tok, "stray case");
    Node *node = new_node(ND_CASE, tok);
    int begin = const_expr(&tok, tok->next);
    int end;
    if (EQUAL(tok, "...")) {
      // [GNU] Case ranges, e.g. "case 1 ... 5:"
      end = const_expr(&tok, tok->next);
      if (end < begin) error_tok(tok, "empty case range specified");
    } else {
      end = begin;
    }
    tok = skip(tok, ':');
    node->label = new_unique_name();
    node->lhs = stmt(rest, tok);
    node->begin = begin;
    node->end = end;
    node->case_next = current_switch->case_next;
    current_switch->case_next = node;
    return node;
  }
  if (EQUAL(tok, "default")) {
    if (!current_switch) error_tok(tok, "stray default");
    Node *node = new_node(ND_CASE, tok);
    tok = skip(tok->next, ':');
    node->label = new_unique_name();
    node->lhs = stmt(rest, tok);
    current_switch->default_case = node;
    return node;
  }
  if (EQUAL(tok, "for")) {
    Node *node = new_node(ND_FOR, tok);
    tok = skip(tok->next, '(');
    enter_scope();
    char *brk = brk_label;
    char *cont = cont_label;
    brk_label = node->brk_label = new_unique_name();
    cont_label = node->cont_label = new_unique_name();
    if (is_typename(tok)) {
      Type *basety = declspec(&tok, tok, NULL);
      node->init = declaration(&tok, tok, basety, NULL);
    } else {
      node->init = expr_stmt(&tok, tok);
    }
    if (!EQUAL(tok, ";")) node->cond = expr(&tok, tok);
    tok = skip(tok, ';');
    if (!EQUAL(tok, ")")) node->inc = expr(&tok, tok);
    tok = skip(tok, ')');
    node->then = stmt(rest, tok);
    leave_scope();
    brk_label = brk;
    cont_label = cont;
    return node;
  }
  if (EQUAL(tok, "while")) {
    Node *node = new_node(ND_FOR, tok);
    tok = skip(tok->next, '(');
    node->cond = expr(&tok, tok);
    tok = skip(tok, ')');
    char *brk = brk_label;
    char *cont = cont_label;
    brk_label = node->brk_label = new_unique_name();
    cont_label = node->cont_label = new_unique_name();
    node->then = stmt(rest, tok);
    brk_label = brk;
    cont_label = cont;
    return node;
  }
  if (EQUAL(tok, "do")) {
    Node *node = new_node(ND_DO, tok);
    char *brk = brk_label;
    char *cont = cont_label;
    brk_label = node->brk_label = new_unique_name();
    cont_label = node->cont_label = new_unique_name();
    node->then = stmt(&tok, tok->next);
    brk_label = brk;
    cont_label = cont;
    if (!EQUAL(tok, "while")) {
      error_tok(tok, "expected while");
    }
    tok = skip(tok->next, '(');
    node->cond = expr(&tok, tok);
    tok = skip(tok, ')');
    *rest = skip(tok, ';');
    return node;
  }
  if (EQUAL(tok, "asm") || EQUAL(tok, "__asm__")) {
    Node *node = new_node(ND_ASM, tok);
    node->azm = asm_stmt(rest, tok);
    return node;
  }
  if (EQUAL(tok, "goto")) {
    if (EQUAL(tok->next, "*")) {
      // [GNU] `goto *ptr` jumps to the address specified by `ptr`.
      Node *node = new_node(ND_GOTO_EXPR, tok);
      node->lhs = expr(&tok, tok->next->next);
      *rest = skip(tok, ';');
      return node;
    }
    Node *node = new_node(ND_GOTO, tok);
    node->label = get_ident(tok->next);
    node->goto_next = gotos;
    gotos = node;
    *rest = skip(tok->next->next, ';');
    return node;
  }
  if (EQUAL(tok, "break")) {
    if (!brk_label) error_tok(tok, "stray break");
    Node *node = new_node(ND_GOTO, tok);
    node->unique_label = brk_label;
    *rest = skip(tok->next, ';');
    return node;
  }
  if (EQUAL(tok, "continue")) {
    if (!cont_label) error_tok(tok, "stray continue");
    Node *node = new_node(ND_GOTO, tok);
    node->unique_label = cont_label;
    *rest = skip(tok->next, ';');
    return node;
  }
  if (tok->kind == TK_IDENT && EQUAL(tok->next, ":")) {
    Node *node = new_node(ND_LABEL, tok);
    node->label = strndup(tok->loc, tok->len);
    node->unique_label = new_unique_name();
    node->lhs = stmt(rest, tok->next->next);
    node->goto_next = labels;
    labels = node;
    return node;
  }
  if (EQUAL(tok, "{")) return compound_stmt(rest, tok->next);
  return expr_stmt(rest, tok);
}

// compound-stmt = (typedef | declaration | stmt)* "}"
static Node *compound_stmt(Token **rest, Token *tok) {
  Node *node = new_node(ND_BLOCK, tok);
  Node head = {};
  Node *cur = &head;
  enter_scope();
  while (!EQUAL(tok, "}")) {
    if (is_typename(tok) && !EQUAL(tok->next, ":")) {
      VarAttr attr = {};
      Type *basety = declspec(&tok, tok, &attr);
      if (attr.is_typedef) {
        tok = parse_typedef(tok, basety);
        continue;
      }
      if (is_function(tok)) {
        tok = function(tok, basety, &attr);
        continue;
      }
      if (attr.is_extern) {
        tok = global_variable(tok, basety, &attr);
        continue;
      }
      cur = cur->next = declaration(&tok, tok, basety, &attr);
    } else {
      cur = cur->next = stmt(&tok, tok);
    }
    add_type(cur);
  }
  leave_scope();
  node->body = head.next;
  *rest = tok->next;
  return node;
}

// expr-stmt = expr? ";"
static Node *expr_stmt(Token **rest, Token *tok) {
  if (EQUAL(tok, ";")) {
    *rest = tok->next;
    return new_node(ND_BLOCK, tok);
  }
  Node *node = new_node(ND_EXPR_STMT, tok);
  node->lhs = expr(&tok, tok);
  *rest = skip(tok, ';');
  return node;
}

// expr = assign ("," expr)?
Node *expr(Token **rest, Token *tok) {
  Node *node = assign(&tok, tok);
  if (EQUAL(tok, ",")) {
    return new_binary(ND_COMMA, node, expr(rest, tok->next), tok);
  }
  *rest = tok;
  return node;
}

int64_t eval(Node *node) {
  return eval2(node, NULL);
}

// Evaluate a given node as a constant expression.
//
// A constant expression is either just a number or ptr+n where ptr
// is a pointer to a global variable and n is a postiive/negative
// number. The latter form is accepted only as an initialization
// expression for a global variable.
int64_t eval2(Node *node, char ***label) {
  int64_t x, y;
  add_type(node);
  if (is_flonum(node->ty)) return eval_double(node);
  switch (node->kind) {
    case ND_ADD:
      return eval2(node->lhs, label) + eval(node->rhs);
    case ND_SUB:
      return eval2(node->lhs, label) - eval(node->rhs);
    case ND_MUL:
      return eval(node->lhs) * eval(node->rhs);
    case ND_DIV:
      y = eval(node->rhs);
      if (!y) error_tok(node->rhs->tok, "constexpr div by zero");
      if (node->ty->is_unsigned) {
        return (uint64_t)eval(node->lhs) / y;
      }
      x = eval(node->lhs);
      if (x == 0x8000000000000000 && y == -1) {
        error_tok(node->rhs->tok, "constexpr divide error");
      }
      return x / y;
    case ND_NEG:
      return -eval(node->lhs);
    case ND_REM:
      y = eval(node->rhs);
      if (!y) error_tok(node->rhs->tok, "constexpr rem by zero");
      if (node->ty->is_unsigned) {
        return (uint64_t)eval(node->lhs) % y;
      }
      return eval(node->lhs) % y;
    case ND_BINAND:
      return eval(node->lhs) & eval(node->rhs);
    case ND_BINOR:
      return eval(node->lhs) | eval(node->rhs);
    case ND_BINXOR:
      return eval(node->lhs) ^ eval(node->rhs);
    case ND_SHL:
      return eval(node->lhs) << eval(node->rhs);
    case ND_SHR:
      if (node->ty->is_unsigned && node->ty->size == 8)
        return (uint64_t)eval(node->lhs) >> eval(node->rhs);
      return eval(node->lhs) >> eval(node->rhs);
    case ND_EQ: {
      bool lhs = is_const_expr(node->lhs);
      bool rhs = is_const_expr(node->rhs);
      if (rhs && node->lhs->kind == ND_VAR && node->lhs->ty->kind == TY_ARRAY &&
          node->lhs->var->is_string_literal) {
        return eval(node->rhs) == (intptr_t)node->lhs->var->init_data;
      } else if (lhs && node->rhs->kind == ND_VAR &&
                 node->rhs->ty->kind == TY_ARRAY &&
                 node->rhs->var->is_string_literal) {
        return eval(node->lhs) == (intptr_t)node->rhs->var->init_data;
      } else {
        return eval(node->lhs) == eval(node->rhs);
      }
    }
    case ND_NE: {
      bool lhs = is_const_expr(node->lhs);
      bool rhs = is_const_expr(node->rhs);
      if (rhs && node->lhs->kind == ND_VAR && node->lhs->ty->kind == TY_ARRAY &&
          node->lhs->var->is_string_literal) {
        return eval(node->rhs) != (intptr_t)node->lhs->var->init_data;
      } else if (lhs && node->rhs->kind == ND_VAR &&
                 node->rhs->ty->kind == TY_ARRAY &&
                 node->rhs->var->is_string_literal) {
        return eval(node->lhs) != (intptr_t)node->rhs->var->init_data;
      } else {
        return eval(node->lhs) != eval(node->rhs);
      }
    }
    case ND_LT:
      if (node->lhs->ty->is_unsigned)
        return (uint64_t)eval(node->lhs) < eval(node->rhs);
      return eval(node->lhs) < eval(node->rhs);
    case ND_LE:
      if (node->lhs->ty->is_unsigned)
        return (uint64_t)eval(node->lhs) <= eval(node->rhs);
      return eval(node->lhs) <= eval(node->rhs);
    case ND_COND:
      return eval(node->cond) ? eval2(node->then, label)
                              : eval2(node->els, label);
    case ND_COMMA:
      return eval2(node->rhs, label);
    case ND_NOT:
      return !eval(node->lhs);
    case ND_BITNOT:
      return ~eval(node->lhs);
    case ND_LOGAND:
      return eval(node->lhs) && eval(node->rhs);
    case ND_LOGOR:
      return eval(node->lhs) || eval(node->rhs);
    case ND_CAST: {
      int64_t val = eval2(node->lhs, label);
      if (is_integer(node->ty)) {
        switch (node->ty->size) {
          case 1:
            return node->ty->is_unsigned ? (uint8_t)val : (int8_t)val;
          case 2:
            return node->ty->is_unsigned ? (uint16_t)val : (int16_t)val;
          case 4:
            return node->ty->is_unsigned ? (uint32_t)val : (int32_t)val;
        }
      }
      return val;
    }
    case ND_ADDR:
      return eval_rval(node->lhs, label);
    case ND_LABEL_VAL:
      *label = &node->unique_label;
      return 0;
    case ND_MEMBER:
      if (!label) error_tok(node->tok, "not a compile-time constant");
      if (node->ty->kind != TY_ARRAY)
        error_tok(node->tok, "invalid initializer");
      return eval_rval(node->lhs, label) + node->member->offset;
    case ND_VAR:
      if (!label) {
        if (node->ty->kind == TY_ARRAY && node->var->is_string_literal) {
          return (intptr_t)node->var->init_data;
        }
        error_tok(node->tok, "not a compile-time constant");
      }
      if (node->var->ty->kind != TY_ARRAY && node->var->ty->kind != TY_FUNC) {
        error_tok(node->tok, "invalid initializer");
      }
      *label = &node->var->name;
      return 0;
    case ND_NUM:
      return node->val;
  }
  error_tok(node->tok, "not a compile-time constant");
}

static int64_t eval_rval(Node *node, char ***label) {
  switch (node->kind) {
    case ND_VAR:
      if (node->var->is_local) {
        error_tok(node->tok, "not a compile-time constant");
      }
      *label = &node->var->name;
      return 0;
    case ND_DEREF:
      return eval2(node->lhs, label);
    case ND_MEMBER:
      return eval_rval(node->lhs, label) + node->member->offset;
  }
  error_tok(node->tok, "invalid initializer");
}

bool is_const_expr(Node *node) {
  add_type(node);
  switch (node->kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_REM:
    case ND_BINAND:
    case ND_BINOR:
    case ND_BINXOR:
    case ND_SHL:
    case ND_SHR:
    case ND_LT:
    case ND_LE:
    case ND_LOGAND:
    case ND_LOGOR:
      return is_const_expr(node->lhs) && is_const_expr(node->rhs);
    case ND_EQ:
    case ND_NE: {
      bool lhs = is_const_expr(node->lhs);
      bool rhs = is_const_expr(node->rhs);
      return (lhs && rhs) ||
             (rhs && node->lhs->kind == ND_VAR &&
              node->lhs->var->is_string_literal) ||
             (lhs && node->rhs->kind == ND_VAR &&
              node->rhs->var->is_string_literal);
    }
    case ND_COND:
      if (!is_const_expr(node->cond)) return false;
      return is_const_expr(eval(node->cond) ? node->then : node->els);
    case ND_COMMA:
      return is_const_expr(node->rhs);
    case ND_NEG:
    case ND_NOT:
    case ND_BITNOT:
    case ND_CAST:
      return is_const_expr(node->lhs);
    case ND_NUM:
      return true;
    case ND_VAR:
      return node->var->is_string_literal;
  }
  return false;
}

int64_t const_expr(Token **rest, Token *tok) {
  Node *node = conditional(rest, tok);
  return eval(node);
}

static double eval_double(Node *node) {
  add_type(node);
  if (is_integer(node->ty)) {
    if (node->ty->is_unsigned) {
      return (unsigned long)eval(node);
    }
    return eval(node);
  }
  switch (node->kind) {
    case ND_ADD:
      return eval_double(node->lhs) + eval_double(node->rhs);
    case ND_SUB:
      return eval_double(node->lhs) - eval_double(node->rhs);
    case ND_MUL:
      return eval_double(node->lhs) * eval_double(node->rhs);
    case ND_DIV:
      return eval_double(node->lhs) / eval_double(node->rhs);
    case ND_NEG:
      return -eval_double(node->lhs);
    case ND_COND:
      return eval_double(node->cond) ? eval_double(node->then)
                                     : eval_double(node->els);
    case ND_COMMA:
      return eval_double(node->rhs);
    case ND_CAST:
      if (is_flonum(node->lhs->ty)) return eval_double(node->lhs);
      return eval(node->lhs);
    case ND_NUM:
      return node->fval;
  }
  error_tok(node->tok, "not a compile-time constant");
}

// Convert op= operators to expressions containing an assignment.
//
// In general, `A op= C` is converted to ``tmp = &A, *tmp = *tmp op B`.
// However, if a given expression is of form `A.x op= C`, the input is
// converted to `tmp = &A, (*tmp).x = (*tmp).x op C` to handle assignments
// to bitfields.
static Node *to_assign(Node *binary) {
  add_type(binary->lhs);
  add_type(binary->rhs);
  Token *tok = binary->tok;
  // Convert `A.x op= C` to `tmp = &A, (*tmp).x = (*tmp).x op C`.
  if (binary->lhs->kind == ND_MEMBER) {
    Obj *var = new_lvar("", pointer_to(binary->lhs->lhs->ty));
    Node *expr1 = new_binary(ND_ASSIGN, new_var_node(var, tok),
                             new_unary(ND_ADDR, binary->lhs->lhs, tok), tok);
    Node *expr2 = new_unary(
        ND_MEMBER, new_unary(ND_DEREF, new_var_node(var, tok), tok), tok);
    expr2->member = binary->lhs->member;
    Node *expr3 = new_unary(
        ND_MEMBER, new_unary(ND_DEREF, new_var_node(var, tok), tok), tok);
    expr3->member = binary->lhs->member;
    Node *expr4 =
        new_binary(ND_ASSIGN, expr2,
                   new_binary(binary->kind, expr3, binary->rhs, tok), tok);
    return new_binary(ND_COMMA, expr1, expr4, tok);
  }
  // If A is an atomic type, Convert `A op= B` to
  //
  // ({
  //   T1 *addr = &A; T2 val = (B); T1 old = *addr; T1 new;
  //   do {
  //    new = old op val;
  //   } while (!atomic_compare_exchange_strong(addr, &old, new));
  //   new;
  // })
  if (binary->lhs->ty->is_atomic) {
    Node head = {};
    Node *cur = &head;
    Obj *addr = new_lvar("", pointer_to(binary->lhs->ty));
    Obj *val = new_lvar("", binary->rhs->ty);
    Obj *old = new_lvar("", binary->lhs->ty);
    Obj *new = new_lvar("", binary->lhs->ty);
    cur = cur->next =
        new_unary(ND_EXPR_STMT,
                  new_binary(ND_ASSIGN, new_var_node(addr, tok),
                             new_unary(ND_ADDR, binary->lhs, tok), tok),
                  tok);
    cur = cur->next = new_unary(
        ND_EXPR_STMT,
        new_binary(ND_ASSIGN, new_var_node(val, tok), binary->rhs, tok), tok);
    cur = cur->next = new_unary(
        ND_EXPR_STMT,
        new_binary(ND_ASSIGN, new_var_node(old, tok),
                   new_unary(ND_DEREF, new_var_node(addr, tok), tok), tok),
        tok);
    Node *loop = new_node(ND_DO, tok);
    loop->brk_label = new_unique_name();
    loop->cont_label = new_unique_name();
    Node *body = new_binary(ND_ASSIGN, new_var_node(new, tok),
                            new_binary(binary->kind, new_var_node(old, tok),
                                       new_var_node(val, tok), tok),
                            tok);
    loop->then = new_node(ND_BLOCK, tok);
    loop->then->body = new_unary(ND_EXPR_STMT, body, tok);
    Node *cas = new_node(ND_CAS, tok);
    cas->cas_addr = new_var_node(addr, tok);
    cas->cas_old = new_unary(ND_ADDR, new_var_node(old, tok), tok);
    cas->cas_new = new_var_node(new, tok);
    loop->cond = new_unary(ND_NOT, cas, tok);
    cur = cur->next = loop;
    cur = cur->next = new_unary(ND_EXPR_STMT, new_var_node(new, tok), tok);
    Node *node = new_node(ND_STMT_EXPR, tok);
    node->body = head.next;
    return node;
  }
  // Convert `A op= B` to ``tmp = &A, *tmp = *tmp op B`.
  Obj *var = new_lvar("", pointer_to(binary->lhs->ty));
  Node *expr1 = new_binary(ND_ASSIGN, new_var_node(var, tok),
                           new_unary(ND_ADDR, binary->lhs, tok), tok);
  Node *expr2 = new_binary(
      ND_ASSIGN, new_unary(ND_DEREF, new_var_node(var, tok), tok),
      new_binary(binary->kind, new_unary(ND_DEREF, new_var_node(var, tok), tok),
                 binary->rhs, tok),
      tok);
  return new_binary(ND_COMMA, expr1, expr2, tok);
}

// @hint it's expr() without the commas
// assign    = conditional (assign-op assign)?
// assign-op = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^="
//           | "<<=" | ">>="
static Node *assign(Token **rest, Token *tok) {
  Node *node = conditional(&tok, tok);
  if (tok->len == 1) {
    if (tok->loc[0] == '=') {
      return new_binary(ND_ASSIGN, node, assign(rest, tok->next), tok);
    }
  } else if (tok->len == 2) {
    if (tok->loc[0] == '+' && tok->loc[1] == '=') {
      return to_assign(new_add(node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '-' && tok->loc[1] == '=') {
      return to_assign(new_sub(node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '*' && tok->loc[1] == '=') {
      return to_assign(new_binary(ND_MUL, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '/' && tok->loc[1] == '=') {
      return to_assign(new_binary(ND_DIV, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '%' && tok->loc[1] == '=') {
      return to_assign(new_binary(ND_REM, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '&' && tok->loc[1] == '=') {
      return to_assign(
          new_binary(ND_BINAND, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '|' && tok->loc[1] == '=') {
      return to_assign(
          new_binary(ND_BINOR, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '^' && tok->loc[1] == '=') {
      return to_assign(
          new_binary(ND_BINXOR, node, assign(rest, tok->next), tok));
    }
  } else if (tok->len == 3) {
    if (tok->loc[0] == '<' && tok->loc[1] == '<' && tok->loc[2] == '=') {
      return to_assign(new_binary(ND_SHL, node, assign(rest, tok->next), tok));
    }
    if (tok->loc[0] == '>' && tok->loc[1] == '>' && tok->loc[2] == '=') {
      return to_assign(new_binary(ND_SHR, node, assign(rest, tok->next), tok));
    }
  }
  *rest = tok;
  return node;
}

// conditional = logor ("?" expr? ":" conditional)?
static Node *conditional(Token **rest, Token *tok) {
  Node *cond = logor(&tok, tok);
  if (!EQUAL(tok, "?")) {
    *rest = tok;
    return cond;
  }
  if (EQUAL(tok->next, ":")) {
    // [GNU] Compile `a ?: b` as `tmp = a, tmp ? tmp : b`.
    Node *els = conditional(rest, tok->next->next);
    if (is_const_expr(cond)) { /* DCE */
      if (is_const_expr_true(cond)) {
        return cond;
      } else {
        return els;
      }
    }
    Obj *var = new_lvar("", cond->ty);
    Node *lhs = new_binary(ND_ASSIGN, new_var_node(var, tok), cond, tok);
    Node *rhs = new_node(ND_COND, tok);
    rhs->cond = new_var_node(var, tok);
    rhs->then = new_var_node(var, tok);
    rhs->els = els;
    return new_binary(ND_COMMA, lhs, rhs, tok);
  }
  Node *node = new_node(ND_COND, tok);
  node->cond = cond;
  node->then = expr(&tok, tok->next);
  tok = skip(tok, ':');
  node->els = conditional(rest, tok);
  add_type(node);
  if (is_const_expr(cond)) { /* DCE */
    if (is_const_expr_true(cond)) {
      return new_cast(node->then, node->ty);
    } else {
      return new_cast(node->els, node->ty);
    }
  }
  return node;
}

// logor = logand ("||" logand)*
static Node *logor(Token **rest, Token *tok) {
  Node *node = logand(&tok, tok);
  while (EQUAL(tok, "||")) {
    Token *start = tok;
    node = new_binary(ND_LOGOR, node, logand(&tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// logand = binor ("&&" binor)*
static Node *logand(Token **rest, Token *tok) {
  Node *node = binor(&tok, tok);
  while (EQUAL(tok, "&&")) {
    Token *start = tok;
    node = new_binary(ND_LOGAND, node, binor(&tok, tok->next), start);
  }
  *rest = tok;
#if 0
  if (node->lhs && node->rhs) {
    add_type(node->lhs);
    add_type(node->rhs);
    if (((is_const_expr(node->lhs) && !is_const_expr_true(node->lhs)) ||
         (is_const_expr(node->rhs) && !is_const_expr_true(node->rhs)))) {
      node->lhs = new_cast(new_num(0, start), node->lhs->ty); /* DCE */
      node->rhs = new_cast(new_num(0, start), node->rhs->ty);
    }
  }
#endif
  return node;
}

// binor = binxor ("|" binxor)*
static Node *binor(Token **rest, Token *tok) {
  Node *node = binxor(&tok, tok);
  while (EQUAL(tok, "|")) {
    Token *start = tok;
    node = new_binary(ND_BINOR, node, binxor(&tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// binxor = binand ("^" binand)*
static Node *binxor(Token **rest, Token *tok) {
  Node *node = binand(&tok, tok);
  while (EQUAL(tok, "^")) {
    Token *start = tok;
    node = new_binary(ND_BINXOR, node, binand(&tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// binand = equality ("&" equality)*
static Node *binand(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);
  while (EQUAL(tok, "&")) {
    Token *start = tok;
    node = new_binary(ND_BINAND, node, equality(&tok, tok->next), start);
  }
  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);
  for (;;) {
    Token *start = tok;
    if (EQUAL(tok, "==")) {
      node = new_binary(ND_EQ, node, relational(&tok, tok->next), start);
      continue;
    }
    if (EQUAL(tok, "!=")) {
      node = new_binary(ND_NE, node, relational(&tok, tok->next), start);
      continue;
    }
    *rest = tok;
    return node;
  }
}

// relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
static Node *relational(Token **rest, Token *tok) {
  Node *node = shift(&tok, tok);
  for (;;) {
    Token *start = tok;
    if (EQUAL(tok, "<")) {
      node = new_binary(ND_LT, node, shift(&tok, tok->next), start);
      continue;
    }
    if (tok->len == 2) {
      if (tok->loc[0] == '<' && tok->loc[1] == '=') {
        node = new_binary(ND_LE, node, shift(&tok, tok->next), start);
        continue;
      }
      if (tok->loc[0] == '>' && tok->loc[1] == '=') {
        node = new_binary(ND_LE, shift(&tok, tok->next), node, start);
        continue;
      }
    } else if (tok->len == 1) {
      if (tok->loc[0] == '>') {
        node = new_binary(ND_LT, shift(&tok, tok->next), node, start);
        continue;
      }
    }
    *rest = tok;
    return node;
  }
}

// shift = add ("<<" add | ">>" add)*
static Node *shift(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);
  for (;;) {
    Token *start = tok;
    if (tok->len == 2) {
      if (tok->loc[0] == '<' && tok->loc[1] == '<') {
        node = new_binary(ND_SHL, node, add(&tok, tok->next), start);
        continue;
      }
      if (tok->loc[0] == '>' && tok->loc[1] == '>') {
        node = new_binary(ND_SHR, node, add(&tok, tok->next), start);
        continue;
      }
    }
    *rest = tok;
    return node;
  }
}

static Node *get_vla_size(Type *ty, Token *tok) {
  if (ty->vla_size) {
    return new_var_node(ty->vla_size, tok);
  } else {
    Node *lhs = compute_vla_size(ty, tok);
    Node *rhs = new_var_node(ty->vla_size, tok);
    return new_binary(ND_COMMA, lhs, rhs, tok);
  }
}

// In C, `+` operator is overloaded to perform the pointer arithmetic.
// If p is a pointer, p+n adds not n but sizeof(*p)*n to the value of p,
// so that p+n points to the location n elements (not bytes) ahead of p.
// In other words, we need to scale an integer value before adding to a
// pointer value. This function takes care of the scaling.
static Node *new_add(Node *lhs, Node *rhs, Token *tok) {
  add_type(lhs);
  add_type(rhs);
  // num + num
  if (is_numeric(lhs->ty) && is_numeric(rhs->ty))
    return new_binary(ND_ADD, lhs, rhs, tok);
  if (lhs->ty->base && rhs->ty->base) error_tok(tok, "invalid operands");
  // Canonicalize `num + ptr` to `ptr + num`.
  if (!lhs->ty->base && rhs->ty->base) {
    Node *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }
  // VLA + num
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(ND_MUL, rhs, get_vla_size(lhs->ty->base, tok), tok);
    return new_binary(ND_ADD, lhs, rhs, tok);
  }
  // ptr + num
  rhs = new_binary(ND_MUL, rhs, new_long(lhs->ty->base->size, tok), tok);
  return new_binary(ND_ADD, lhs, rhs, tok);
}

// Like `+`, `-` is overloaded for the pointer type.
static Node *new_sub(Node *lhs, Node *rhs, Token *tok) {
  add_type(lhs);
  add_type(rhs);
  // num - num
  if (is_numeric(lhs->ty) && is_numeric(rhs->ty))
    return new_binary(ND_SUB, lhs, rhs, tok);
  // VLA + num
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(ND_MUL, rhs, get_vla_size(lhs->ty->base, tok), tok);
    add_type(rhs);
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);
    node->ty = lhs->ty;
    return node;
  }
  // ptr - num
  if (lhs->ty->base && is_integer(rhs->ty)) {
    rhs = new_binary(ND_MUL, rhs, new_long(lhs->ty->base->size, tok), tok);
    add_type(rhs);
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);
    node->ty = lhs->ty;
    return node;
  }
  // ptr - ptr, which returns how many elements are between the two.
  if (lhs->ty->base && rhs->ty->base) {
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);
    node->ty = ty_long;
    return new_binary(ND_DIV, node, new_num(lhs->ty->base->size, tok), tok);
  }
  error_tok(tok, "invalid operands");
}

static Node *new_mul(Node *lhs, Node *rhs, Token *tok) {
  return new_binary(ND_MUL, lhs, rhs, tok);
}

// add = mul ("+" mul | "-" mul)*
static Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);
  for (;;) {
    Token *start = tok;
    if (tok->len == 1) {
      if (tok->loc[0] == '+') {
        node = new_add(node, mul(&tok, tok->next), start);
        continue;
      }
      if (tok->loc[0] == '-') {
        node = new_sub(node, mul(&tok, tok->next), start);
        continue;
      }
    }
    *rest = tok;
    return node;
  }
}

// mul = cast ("*" cast | "/" cast | "%" cast)*
static Node *mul(Token **rest, Token *tok) {
  Node *node = cast(&tok, tok);
  for (;;) {
    Token *start = tok;
    if (tok->len == 1) {
      if (tok->loc[0] == '*') {
        node = new_mul(node, cast(&tok, tok->next), start);
        continue;
      }
      if (tok->loc[0] == '/') {
        node = new_binary(ND_DIV, node, cast(&tok, tok->next), start);
        continue;
      }
      if (tok->loc[0] == '%') {
        node = new_binary(ND_REM, node, cast(&tok, tok->next), start);
        continue;
      }
    }
    *rest = tok;
    return node;
  }
}

// cast = "(" type-name ")" cast | unary
static Node *cast(Token **rest, Token *tok) {
  if (EQUAL(tok, "(") && is_typename(tok->next)) {
    Token *start = tok;
    Type *ty = typename(&tok, tok->next);
    tok = skip(tok, ')');
    // compound literal
    if (EQUAL(tok, "{")) return unary(rest, start);
    // type cast
    Node *node = new_cast(cast(rest, tok), ty);
    node->tok = start;
    return node;
  }
  return unary(rest, tok);
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~") cast
//       | ("++" | "--") unary
//       | "&&" ident
//       | postfix
static Node *unary(Token **rest, Token *tok) {
  if (tok->len == 1) {
    if (tok->loc[0] == '+') {
      return cast(rest, tok->next);
    }
    if (tok->loc[0] == '-') {
      return new_unary(ND_NEG, cast(rest, tok->next), tok);
    }
    if (tok->loc[0] == '&') {
      Node *lhs = cast(rest, tok->next);
      add_type(lhs);
      if (lhs->kind == ND_MEMBER && lhs->member->is_bitfield) {
        error_tok(tok, "cannot take address of bitfield");
      }
      return new_unary(ND_ADDR, lhs, tok);
    }
    if (tok->loc[0] == '*') {
      // [https://www.sigbus.info/n1570#6.5.3.2p4] This is an oddity
      // in the C spec, but dereferencing a function shouldn't do
      // anything. If foo is a function, `*foo`, `**foo` or `*****foo`
      // are all equivalent to just `foo`.
      Node *node = cast(rest, tok->next);
      add_type(node);
      if (node->ty->kind == TY_FUNC) return node;
      return new_unary(ND_DEREF, node, tok);
    }
    if (tok->loc[0] == '!') {
      return new_unary(ND_NOT, cast(rest, tok->next), tok);
    }
    if (tok->loc[0] == '~') {
      return new_unary(ND_BITNOT, cast(rest, tok->next), tok);
    }
  } else if (tok->len == 2) {
    // Read ++i as i+=1
    if (tok->loc[0] == '+' && tok->loc[1] == '+') {
      return to_assign(new_add(unary(rest, tok->next), new_num(1, tok), tok));
    }
    // Read --i as i-=1
    if (tok->loc[0] == '-' && tok->loc[1] == '-') {
      return to_assign(new_sub(unary(rest, tok->next), new_num(1, tok), tok));
    }
    // [GNU] labels-as-values
    if (tok->loc[0] == '&' && tok->loc[1] == '&') {
      Node *node = new_node(ND_LABEL_VAL, tok);
      node->label = get_ident(tok->next);
      node->goto_next = gotos;
      gotos = node;
      *rest = tok->next->next;
      return node;
    }
  }
  return postfix(rest, tok);
}

// struct-members = (declspec declarator (","  declarator)* ";" javadown?)*
static void struct_members(Token **rest, Token *tok, Type *ty) {
  Member head = {};
  Member *cur = &head;
  int idx = 0;
  while (!EQUAL(tok, "}")) {
    VarAttr attr = {};
    Type *basety = declspec(&tok, tok, &attr);
    bool first = true;
    // Anonymous struct member
    if ((basety->kind == TY_STRUCT || basety->kind == TY_UNION) &&
        CONSUME(&tok, tok, ";")) {
      Member *mem = calloc(1, sizeof(Member));
      mem->ty = basety;
      mem->idx = idx++;
      mem->align = attr.align ? attr.align : mem->ty->align;
      cur = cur->next = mem;
      continue;
    }
    // Regular struct members
    while (!CONSUME(&tok, tok, ";")) {
      if (!first) tok = skip(tok, ',');
      if (tok->kind == TK_JAVADOWN) {
        current_javadown = tok;
        tok = tok->next;
      }
      first = false;
      Member *mem = calloc(1, sizeof(Member));
      mem->ty = declarator(&tok, tok, basety);
      mem->name = mem->ty->name;
      mem->idx = idx++;
      mem->align = attr.align ? attr.align : mem->ty->align;
      if (CONSUME(&tok, tok, ":")) {
        mem->is_bitfield = true;
        mem->bit_width = const_expr(&tok, tok);
      }
      cur = cur->next = mem;
    }
    if (tok->kind == TK_JAVADOWN) {
      tok = tok->next;
    }
  }
  // If the last element is an array of incomplete type, it's
  // called a "flexible array member". It should behave as if
  // if were a zero-sized array.
  if (cur != &head && cur->ty->kind == TY_ARRAY && cur->ty->array_len < 0) {
    cur->ty = array_of(cur->ty->base, 0);
    ty->is_flexible = true;
  }
  *rest = tok->next;
  ty->members = head.next;
}

char *ConsumeStringLiteral(Token **rest, Token *tok) {
  char *s;
  if (tok->kind != TK_STR || tok->ty->base->kind != TY_CHAR) {
    error_tok(tok, "expected string literal");
  }
  s = tok->str;
  *rest = tok->next;
  return s;
}

// struct-union-decl = attribute? ident? ("{" struct-members)?
static Type *struct_union_decl(Token **rest, Token *tok) {
  Type *ty = struct_type();
  tok = attribute_list(tok, ty, type_attributes);
  // Read a tag.
  Token *tag = NULL;
  if (tok->kind == TK_IDENT) {
    tag = tok;
    tok = tok->next;
  }
  if (tag && !EQUAL(tok, "{")) {
    *rest = tok;
    Type *ty2 = find_tag(tag);
    if (ty2) return ty2;
    ty->size = -1;
    push_tag_scope(tag, ty);
    return ty;
  }
  ty->name = tag;
  tok = skip(tok, '{');
  // Construct a struct object.
  if (tok->kind == TK_JAVADOWN) {
    current_javadown = tok;
    tok = tok->next;
  }
  struct_members(&tok, tok, ty);
  *rest = attribute_list(tok, ty, type_attributes);
  if (tag) {
    // If this is a redefinition, overwrite a previous type.
    // Otherwise, register the struct type.
    Type *ty2 = hashmap_get2(&scope->tags, tag->loc, tag->len);
    if (ty2) {
      *ty2 = *ty;
      return ty2;
    }
    push_tag_scope(tag, ty);
  }
  return ty;
}

// struct-decl = struct-union-decl
static Type *struct_decl(Token **rest, Token *tok) {
  Type *ty = struct_union_decl(rest, tok);
  ty->kind = TY_STRUCT;
  if (ty->size < 0) return ty;
  // Assign offsets within the struct to members.
  int bits = 0;
  for (Member *m = ty->members; m; m = m->next) {
    if (m->is_bitfield && m->bit_width == 0) {
      // Zero-width anonymous bitfield has a special meaning.
      // It affects only alignment.
      bits = ROUNDUP(bits, m->ty->size * 8);
    } else if (m->is_bitfield) {
      int sz = m->ty->size;
      if (bits / (sz * 8) != (bits + m->bit_width - 1) / (sz * 8)) {
        bits = ROUNDUP(bits, sz * 8);
      }
      m->offset = ROUNDDOWN(bits / 8, sz);
      m->bit_offset = bits % (sz * 8);
      bits += m->bit_width;
    } else {
      if (!ty->is_packed) {
        bits = ROUNDUP(bits, m->align * 8);
      }
      m->offset = bits / 8;
      bits += m->ty->size * 8;
    }
    if (!ty->is_packed && !ty->is_aligned && ty->align < m->align) {
      ty->align = m->align;
    }
  }
  ty->size = ROUNDUP(bits, ty->align * 8) / 8;
  return ty;
}

// union-decl = struct-union-decl
static Type *union_decl(Token **rest, Token *tok) {
  Type *ty = struct_union_decl(rest, tok);
  ty->kind = TY_UNION;
  if (ty->size < 0) return ty;
  // If union, we don't have to assign offsets because they
  // are already initialized to zero. We need to compute the
  // alignment and the size though.
  for (Member *mem = ty->members; mem; mem = mem->next) {
    if (ty->align < mem->align) ty->align = mem->align;
    if (ty->size < mem->ty->size) ty->size = mem->ty->size;
  }
  ty->size = ROUNDUP(ty->size, ty->align);
  return ty;
}

// Find a struct member by name.
static Member *get_struct_member(Type *ty, Token *tok) {
  for (Member *mem = ty->members; mem; mem = mem->next) {
    // Anonymous struct member
    if ((mem->ty->kind == TY_STRUCT || mem->ty->kind == TY_UNION) &&
        !mem->name) {
      if (get_struct_member(mem->ty, tok)) return mem;
      continue;
    }
    // Regular struct member
    if (mem->name->len == tok->len &&
        !strncmp(mem->name->loc, tok->loc, tok->len)) {
      return mem;
    }
  }
  return NULL;
}

// Create a node representing a struct member access, such as foo.bar
// where foo is a struct and bar is a member name.
//
// C has a feature called "anonymous struct" which allows a struct to
// have another unnamed struct as a member like this:
//
//   struct { struct { int a; }; int b; } x;
//
// The members of an anonymous struct belong to the outer struct's
// member namespace. Therefore, in the above example, you can access
// member "a" of the anonymous struct as "x.a".
//
// This function takes care of anonymous structs.
static Node *struct_ref(Node *node, Token *tok) {
  add_type(node);
  if (node->ty->kind != TY_STRUCT && node->ty->kind != TY_UNION)
    error_tok(node->tok, "not a struct nor a union");
  Type *ty = node->ty;
  for (;;) {
    Member *mem = get_struct_member(ty, tok);
    if (!mem) error_tok(tok, "no such member");
    node = new_unary(ND_MEMBER, node, tok);
    node->member = mem;
    if (mem->name) break;
    ty = mem->ty;
  }
  return node;
}

// Convert A++ to `(typeof A)((A += 1) - 1)`
static Node *new_inc_dec(Node *node, Token *tok, int addend) {
  add_type(node);
  return new_cast(new_add(to_assign(new_add(node, new_num(addend, tok), tok)),
                          new_num(-addend, tok), tok),
                  node->ty);
}

// postfix = "(" type-name ")" "{" initializer-list "}"
//         | ident "(" func-args ")" postfix-tail*
//         | primary postfix-tail*
//
// postfix-tail = "[" expr "]"
//              | "(" func-args ")"
//              | "." ident
//              | "->" ident
//              | "++"
//              | "--"
static Node *postfix(Token **rest, Token *tok) {
  if (EQUAL(tok, "(") && is_typename(tok->next)) {
    // Compound literal
    Token *start = tok;
    Type *ty = typename(&tok, tok->next);
    tok = skip(tok, ')');
    if (scope->next == NULL) {
      Obj *var = new_anon_gvar(ty);
      gvar_initializer(rest, tok, var);
      return new_var_node(var, start);
    }
    Obj *var = new_lvar("", ty);
    Node *lhs = lvar_initializer(rest, tok, var);
    Node *rhs = new_var_node(var, tok);
    return new_binary(ND_COMMA, lhs, rhs, start);
  }
  Node *node = primary(&tok, tok);
  for (;;) {
    if (tok->len == 1) {
      if (tok->loc[0] == '(') {
        node = funcall(&tok, tok->next, node);
        continue;
      }
      if (tok->loc[0] == '[') {
        // x[y] is short for *(x+y)
        Token *start = tok;
        Node *idx = expr(&tok, tok->next);
        tok = skip(tok, ']');
        node = new_unary(ND_DEREF, new_add(node, idx, start), start);
        continue;
      }
      if (tok->loc[0] == '.') {
        node = struct_ref(node, tok->next);
        tok = tok->next->next;
        continue;
      }
    } else if (tok->len == 2) {
      if (tok->loc[0] == '-' && tok->loc[1] == '>') {
        // x->y is short for (*x).y
        node = new_unary(ND_DEREF, node, tok);
        node = struct_ref(node, tok->next);
        tok = tok->next->next;
        continue;
      }
      if (tok->loc[0] == '+' && tok->loc[1] == '+') {
        node = new_inc_dec(node, tok, 1);
        tok = tok->next;
        continue;
      }
      if (tok->loc[0] == '-' && tok->loc[1] == '-') {
        node = new_inc_dec(node, tok, -1);
        tok = tok->next;
        continue;
      }
    }
    *rest = tok;
    return node;
  }
}

// funcall = (assign ("," assign)*)? ")"
static Node *funcall(Token **rest, Token *tok, Node *fn) {
  add_type(fn);
  if (fn->ty->kind != TY_FUNC &&
      (fn->ty->kind != TY_PTR || fn->ty->base->kind != TY_FUNC)) {
    error_tok(fn->tok, "not a function");
  }
  Type *ty = (fn->ty->kind == TY_FUNC) ? fn->ty : fn->ty->base;
  Type *param_ty = ty->params;
  Node head = {};
  Node *cur = &head;
  while (!EQUAL(tok, ")")) {
    if (cur != &head) tok = skip(tok, ',');
    Node *arg = assign(&tok, tok);
    add_type(arg);
    if (!param_ty && !ty->is_variadic) error_tok(tok, "too many arguments");
    if (param_ty) {
      if (param_ty->kind != TY_STRUCT && param_ty->kind != TY_UNION) {
        arg = new_cast(arg, param_ty);
      }
      param_ty = param_ty->next;
    } else if (arg->ty->kind == TY_FLOAT) {
      // If parameter type is omitted (e.g. in "..."), float
      // arguments are promoted to double.
      arg = new_cast(arg, ty_double);
    }
    cur = cur->next = arg;
  }
  if (param_ty) error_tok(tok, "too few arguments");
  *rest = skip(tok, ')');
  Node *node = new_unary(ND_FUNCALL, fn, tok);
  node->func_ty = ty;
  node->ty = ty->return_ty;
  node->args = head.next;
  // If a function returns a struct, it is caller's responsibility
  // to allocate a space for the return value.
  if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION) {
    node->ret_buffer = new_lvar("", node->ty);
  }
  return node;
}

// generic-selection = "(" assign "," generic-assoc ("," generic-assoc)* ")"
//
// generic-assoc = type-name ":" assign
//               | "default" ":" assign
static Node *generic_selection(Token **rest, Token *tok) {
  Token *start = tok;
  tok = skip(tok, '(');
  Node *ctrl = assign(&tok, tok);
  add_type(ctrl);
  Type *t1 = ctrl->ty;
  if (t1->kind == TY_FUNC)
    t1 = pointer_to(t1);
  else if (t1->kind == TY_ARRAY)
    t1 = pointer_to(t1->base);
  Node *ret = NULL;
  while (!CONSUME(rest, tok, ")")) {
    tok = skip(tok, ',');
    if (EQUAL(tok, "default")) {
      tok = skip(tok->next, ':');
      Node *node = assign(&tok, tok);
      if (!ret) ret = node;
      continue;
    }
    Type *t2 = typename(&tok, tok);
    tok = skip(tok, ':');
    Node *node = assign(&tok, tok);
    if (is_compatible(t1, t2)) ret = node;
  }
  if (!ret)
    error_tok(start, "controlling expression type not compatible with"
                     " any generic association type");
  return ret;
}

static Node *ParseAtomic2(NodeKind kind, Token *tok, Token **rest) {
  Node *node = new_node(kind, tok);
  tok = skip(tok->next, '(');
  node->lhs = assign(&tok, tok);
  add_type(node->lhs);
  node->ty = node->lhs->ty->base;
  tok = skip(tok, ',');
  node->memorder = const_expr(&tok, tok);
  *rest = skip(tok, ')');
  return node;
}

static Node *ParseAtomic3(NodeKind kind, Token *tok, Token **rest) {
  Node *node = new_node(kind, tok);
  tok = skip(tok->next, '(');
  node->lhs = assign(&tok, tok);
  add_type(node->lhs);
  node->ty = node->lhs->ty->base;
  tok = skip(tok, ',');
  node->rhs = assign(&tok, tok);
  add_type(node->rhs);
  tok = skip(tok, ',');
  node->memorder = const_expr(&tok, tok);
  *rest = skip(tok, ')');
  return node;
}

// primary = "(" "{" stmt+ "}" ")"
//         | "(" expr ")"
//         | "sizeof" "(" type-name ")"
//         | "sizeof" unary
//         | "_Alignof" "(" type-name ")"
//         | "_Alignof" unary
//         | "_Generic" generic-selection
//         | "__builtin_constant_p" "(" expr ")"
//         | "__builtin_reg_class" "(" type-name ")"
//         | "__builtin_types_compatible_p" "(" type-name, type-name, ")"
//         | ident
//         | str
//         | num
static Node *primary(Token **rest, Token *tok) {
  Token *start;
  unsigned char kw;
  start = tok;
  if ((kw = GetKw(tok->loc, tok->len))) {
    if (kw == KW_LP && EQUAL(tok->next, "{")) {
      // This is a GNU statement expresssion.
      Node *node = new_node(ND_STMT_EXPR, tok);
      node->body = compound_stmt(&tok, tok->next->next)->body;
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW_LP) {
      Node *node = expr(&tok, tok->next);
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW_SIZEOF && EQUAL(tok->next, "(") &&
        is_typename(tok->next->next)) {
      Type *ty = typename(&tok, tok->next->next);
      *rest = skip(tok, ')');
      if (ty->kind == TY_VLA) {
        if (ty->vla_size) {
          return new_var_node(ty->vla_size, tok);
        }
        Node *lhs = compute_vla_size(ty, tok);
        Node *rhs = new_var_node(ty->vla_size, tok);
        return new_binary(ND_COMMA, lhs, rhs, tok);
      }
      return new_ulong(ty->size, start);
    }
    if (kw == KW_SIZEOF) {
      Node *node = unary(rest, tok->next);
      add_type(node);
      if (node->ty->kind == TY_VLA) {
        return get_vla_size(node->ty, tok);
      }
      return new_ulong(node->ty->size, tok);
    }
    if ((kw == KW__ALIGNOF || kw == KW___ALIGNOF__) && EQUAL(tok->next, "(") &&
        is_typename(tok->next->next)) {
      Type *ty = typename(&tok, tok->next->next);
      *rest = skip(tok, ')');
      return new_ulong(ty->align, tok);
    }
    if ((kw == KW__ALIGNOF || kw == KW___ALIGNOF__)) {
      Node *node = unary(rest, tok->next);
      add_type(node);
      return new_ulong(node->ty->align, tok);
    }
    if (kw == KW__GENERIC) {
      return generic_selection(rest, tok->next);
    }
    if (kw == KW___BUILTIN_CONSTANT_P) {
      tok = skip(tok->next, '(');
      Node *e = assign(&tok, tok);
      *rest = skip(tok, ')');
      return new_bool(is_const_expr(e), start); /* DCE */
    }
    if (kw == KW___BUILTIN_TYPES_COMPATIBLE_P) {
      tok = skip(tok->next, '(');
      Type *t1 = typename(&tok, tok);
      tok = skip(tok, ',');
      Type *t2 = typename(&tok, tok);
      *rest = skip(tok, ')');
      return new_bool(is_compatible(t1, t2), start);
    }
    if (kw == KW___BUILTIN_OFFSETOF) {
      tok = skip(tok->next, '(');
      Token *stok = tok;
      Type *tstruct = typename(&tok, tok);
      if (tstruct->kind != TY_STRUCT && tstruct->kind != TY_UNION) {
        error_tok(stok, "not a structure or union type");
      }
      tok = skip(tok, ',');
      Token *member = tok;
      tok = tok->next;
      *rest = skip(tok, ')');
      for (Member *m = tstruct->members; m; m = m->next) {
        if (m->name->len == member->len &&
            !memcmp(m->name->loc, member->loc, m->name->len)) {
          return new_ulong(m->offset, start);
        }
      }
      error_tok(member, "no such member");
    }
    if (kw == KW___BUILTIN_REG_CLASS) {
      tok = skip(tok->next, '(');
      Type *ty = typename(&tok, tok);
      *rest = skip(tok, ')');
      if (is_integer(ty) || ty->kind == TY_PTR) return new_int(0, start);
      if (is_flonum(ty)) return new_int(1, start);
      return new_int(2, start);
    }
    if (kw == KW___ATOMIC_COMPARE_EXCHANGE_N) {
      Node *node = new_node(ND_CAS, tok);
      tok = skip(tok->next, '(');
      node->cas_addr = assign(&tok, tok);
      add_type(node->cas_addr);
      tok = skip(tok, ',');
      node->cas_old = assign(&tok, tok);
      add_type(node->cas_old);
      tok = skip(tok, ',');
      node->cas_new = assign(&tok, tok);
      add_type(node->cas_new);
      tok = skip(tok, ',');
      /* weak = */ const_expr(&tok, tok);
      tok = skip(tok, ',');
      node->memorder = const_expr(&tok, tok);
      tok = skip(tok, ',');
      /* memorder_failure = */ const_expr(&tok, tok);
      *rest = skip(tok, ')');
      node->ty = node->cas_addr->ty->base;
      return node;
    }
    if (kw == KW___ATOMIC_EXCHANGE_N) {
      return ParseAtomic3(ND_EXCH_N, tok, rest);
    }
    if (kw == KW___ATOMIC_LOAD) {
      return ParseAtomic3(ND_LOAD, tok, rest);
    }
    if (kw == KW___ATOMIC_STORE) {
      return ParseAtomic3(ND_STORE, tok, rest);
    }
    if (kw == KW___ATOMIC_LOAD_N) {
      return ParseAtomic2(ND_LOAD_N, tok, rest);
    }
    if (kw == KW___ATOMIC_STORE_N) {
      return ParseAtomic3(ND_STORE_N, tok, rest);
    }
    if (kw == KW___ATOMIC_FETCH_ADD) {
      return ParseAtomic3(ND_FETCHADD, tok, rest);
    }
    if (kw == KW___ATOMIC_FETCH_SUB) {
      return ParseAtomic3(ND_FETCHSUB, tok, rest);
    }
    if (kw == KW___ATOMIC_FETCH_XOR) {
      return ParseAtomic3(ND_FETCHXOR, tok, rest);
    }
    if (kw == KW___ATOMIC_FETCH_AND) {
      return ParseAtomic3(ND_FETCHAND, tok, rest);
    }
    if (kw == KW___ATOMIC_FETCH_OR) {
      return ParseAtomic3(ND_FETCHOR, tok, rest);
    }
    if (kw == KW___ATOMIC_TEST_AND_SET) {
      return ParseAtomic2(ND_TESTANDSETA, tok, rest);
    }
    if (kw == KW___ATOMIC_CLEAR) {
      return ParseAtomic2(ND_CLEAR, tok, rest);
    }
    if (kw == KW___SYNC_LOCK_TEST_AND_SET) {
      // TODO(jart): delete me
      Node *node = new_node(ND_TESTANDSET, tok);
      tok = skip(tok->next, '(');
      node->lhs = assign(&tok, tok);
      add_type(node->lhs);
      node->ty = node->lhs->ty->base;
      tok = skip(tok, ',');
      node->rhs = assign(&tok, tok);
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___SYNC_LOCK_RELEASE) {
      // TODO(jart): delete me
      Node *node = new_node(ND_RELEASE, tok);
      tok = skip(tok->next, '(');
      node->lhs = assign(&tok, tok);
      add_type(node->lhs);
      node->ty = node->lhs->ty->base;
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_IA32_MOVNTDQ) {
      Node *node = new_node(ND_MOVNTDQ, tok);
      tok = skip(tok->next, '(');
      node->lhs = assign(&tok, tok);
      add_type(node->lhs);
      node->ty = node->lhs->ty->base;
      tok = skip(tok, ',');
      node->rhs = assign(&tok, tok);
      add_type(node->rhs);
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_IA32_PMOVMSKB128) {
      Node *node = new_node(ND_PMOVMSKB, tok);
      tok = skip(tok->next, '(');
      node->lhs = assign(&tok, tok);
      add_type(node->lhs);
      node->ty = node->lhs->ty->base;
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_EXPECT) { /* do nothing */
      tok = skip(tok->next, '(');
      Node *node = assign(&tok, tok);
      tok = skip(tok, ',');
      const_expr(&tok, tok);
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_ASSUME_ALIGNED) { /* do nothing */
      tok = skip(tok->next, '(');
      Node *node = assign(&tok, tok);
      tok = skip(tok, ',');
      const_expr(&tok, tok);
      if (EQUAL(tok, ",")) {
        const_expr(&tok, tok->next);
      }
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_FPCLASSIFY) {
      Node *node = new_node(ND_FPCLASSIFY, tok);
      node->fpc = calloc(1, sizeof(FpClassify));
      node->ty = ty_int;
      tok = skip(tok->next, '(');
      for (int i = 0; i < 5; ++i) {
        node->fpc->args[i] = const_expr(&tok, tok);
        tok = skip(tok, ',');
      }
      node->fpc->node = expr(&tok, tok);
      add_type(node->fpc->node);
      if (!is_flonum(node->fpc->node->ty)) {
        error_tok(node->fpc->node->tok, "need floating point");
      }
      *rest = skip(tok, ')');
      return node;
    }
    if (kw == KW___BUILTIN_POPCOUNT) {
      Token *t = skip(tok->next, '(');
      Node *node = assign(&t, t);
      if (is_const_expr(node)) {
        *rest = skip(t, ')');
        return new_num(__builtin_popcount(eval(node)), t);
      }
    }
    if (kw == KW___BUILTIN_POPCOUNTL || kw == KW___BUILTIN_POPCOUNTLL) {
      Token *t = skip(tok->next, '(');
      Node *node = assign(&t, t);
      if (is_const_expr(node)) {
        *rest = skip(t, ')');
        return new_num(__builtin_popcountl(eval(node)), t);
      }
    }
    if (kw == KW___BUILTIN_FFS) {
      Token *t = skip(tok->next, '(');
      Node *node = assign(&t, t);
      if (is_const_expr(node)) {
        *rest = skip(t, ')');
        return new_num(__builtin_ffs(eval(node)), t);
      }
    }
    if (kw == KW___BUILTIN_FFSL || kw == KW___BUILTIN_FFSLL) {
      Token *t = skip(tok->next, '(');
      Node *node = assign(&t, t);
      if (is_const_expr(node)) {
        *rest = skip(t, ')');
        return new_num(__builtin_ffsl(eval(node)), t);
      }
    }
    if ((kw == KW___BUILTIN_STRLEN || (!opt_no_builtin && kw == KW_STRLEN)) &&
        EQUAL(tok->next, "(") && tok->next->next->kind == TK_STR &&
        EQUAL(tok->next->next->next, ")")) {
      *rest = tok->next->next->next->next;
      return new_num(strlen(tok->next->next->str), tok);
    }
    if ((kw == KW___BUILTIN_STRPBRK || (!opt_no_builtin && kw == KW_STRPBRK))) {
      if (EQUAL(tok->next, "(") && tok->next->next->kind == TK_STR &&
          EQUAL(tok->next->next->next, ",") &&
          tok->next->next->next->next->kind == TK_STR &&
          EQUAL(tok->next->next->next->next->next, ")")) {
        *rest = tok->next->next->next->next->next->next;
        char *res =
            strpbrk(tok->next->next->str, tok->next->next->next->next->str);
        if (res) {
          return new_var_node(
              new_string_literal(res, array_of(ty_char, strlen(res) + 1)), tok);
        } else {
          return new_num(0, tok);
        }
      }
    }
    if (kw == KW___BUILTIN_STRSTR || (!opt_no_builtin && kw == KW_STRSTR)) {
      if (EQUAL(tok->next, "(") && tok->next->next->kind == TK_STR &&
          EQUAL(tok->next->next->next, ",") &&
          tok->next->next->next->next->kind == TK_STR &&
          EQUAL(tok->next->next->next->next->next, ")")) {
        *rest = tok->next->next->next->next->next->next;
        char *res =
            strstr(tok->next->next->str, tok->next->next->next->next->str);
        if (res) {
          return new_var_node(
              new_string_literal(res, array_of(ty_char, strlen(res) + 1)), tok);
        } else {
          return new_num(0, tok);
        }
      }
    }
    if (kw == KW___BUILTIN_STRCHR || (!opt_no_builtin && kw == KW_STRCHR)) {
      if (EQUAL(tok->next, "(") && tok->next->next->kind == TK_STR &&
          EQUAL(tok->next->next->next, ",") &&
          tok->next->next->next->next->kind == TK_NUM &&
          EQUAL(tok->next->next->next->next->next, ")")) {
        *rest = tok->next->next->next->next->next->next;
        char *res =
            strchr(tok->next->next->str, tok->next->next->next->next->val);
        if (res) {
          return new_var_node(
              new_string_literal(res, array_of(ty_char, strlen(res) + 1)), tok);
        } else {
          return new_num(0, tok);
        }
      }
    }
  }
  if (tok->kind == TK_IDENT) {
    // Variable or enum constant
    VarScope *sc = find_var(tok);
    // [jart] support implicit function declarations with `long` type
    if (!sc && EQUAL(tok->next, "(")) {
      Type *ty = func_type(ty_long);
      ty->is_variadic = true;
      Obj *fn = new_var(strndup(tok->loc, tok->len), ty);
      fn->next = globals;
      fn->is_function = true;
      globals = fn;
      sc = find_var(tok);
    }
    *rest = tok->next;
    // For "static inline" function
    if (sc && sc->var && sc->var->is_function) {
      if (current_fn) {
        strarray_push(&current_fn->refs, sc->var->name);
      } else {
        sc->var->is_root = true;
      }
    }
    if (sc) {
      if (sc->var) return new_var_node(sc->var, tok);
      if (sc->enum_ty) return new_num(sc->enum_val, tok);
    }
    if (EQUAL(tok->next, "(")) {
      error_tok(tok, "implicit declaration of a function");
    }
    error_tok(tok, "undefined variable");
  }
  if (tok->kind == TK_STR) {
    Obj *var = new_string_literal(tok->str, tok->ty);
    *rest = tok->next;
    return new_var_node(var, tok);
  }
  if (tok->kind == TK_NUM) {
    Node *node;
    if (is_flonum(tok->ty)) {
      node = new_node(ND_NUM, tok);
      node->fval = tok->fval;
    } else {
      node = new_num(tok->val, tok);
    }
    node->ty = tok->ty;
    *rest = tok->next;
    return node;
  }
  error_tok(tok, "expected an expression");
}

static Token *parse_typedef(Token *tok, Type *basety) {
  bool first = true;
  while (!CONSUME(&tok, tok, ";")) {
    if (!first) {
      tok = skip(tok, ',');
    }
    first = false;
    Type *ty = declarator(&tok, tok, basety);
    if (!ty->name) error_tok(ty->name_pos, "typedef name omitted");
    tok = attribute_list(tok, ty, type_attributes);
    push_scope(get_ident(ty->name))->type_def = ty;
  }
  return tok;
}

static void create_param_lvars(Type *param) {
  if (param) {
    create_param_lvars(param->next);
    if (!param->name) error_tok(param->name_pos, "parameter name omitted");
    new_lvar(get_ident(param->name), param);
  }
}

// This function matches gotos or labels-as-values with labels.
//
// We cannot resolve gotos as we parse a function because gotos
// can refer a label that appears later in the function.
// So, we need to do this after we parse the entire function.
static void resolve_goto_labels(void) {
  for (Node *x = gotos; x; x = x->goto_next) {
    for (Node *y = labels; y; y = y->goto_next) {
      if (!strcmp(x->label, y->label)) {
        x->unique_label = y->unique_label;
        break;
      }
    }
    if (x->unique_label == NULL) {
      error_tok(x->tok->next, "use of undeclared label");
    }
  }
  gotos = labels = NULL;
}

static Obj *find_func(char *name) {
  Scope *sc = scope;
  while (sc->next) sc = sc->next;
  VarScope *sc2 = hashmap_get(&sc->vars, name);
  if (sc2 && sc2->var && sc2->var->is_function) return sc2->var;
  return NULL;
}

static void mark_live(Obj *var) {
  int i;
  Obj *fn;
  if (!var->is_function || var->is_live) return;
  var->is_live = true;
  for (i = 0; i < var->refs.len; i++) {
    fn = find_func(var->refs.data[i]);
    if (fn) mark_live(fn);
  }
}

static Token *function(Token *tok, Type *basety, VarAttr *attr) {
  Type *ty = declarator(&tok, tok, basety);
  if (!ty->name) error_tok(ty->name_pos, "function name omitted");
  char *name_str = get_ident(ty->name);
  Obj *fn = find_func(name_str);
  if (fn) {
    // Redeclaration
    if (!fn->is_function)
      error_tok(tok, "redeclared as a different kind of symbol");
    if (fn->is_definition && EQUAL(tok, "{"))
      error_tok(tok, "redefinition of %s", name_str);
    if (!fn->is_static && attr->is_static)
      error_tok(tok, "static declaration follows a non-static declaration");
    fn->is_definition = fn->is_definition || EQUAL(tok, "{");
    fn->is_weak |= attr->is_weak;
    fn->is_noreturn |= attr->is_noreturn;
    fn->tok = ty->name;
  } else {
    fn = new_gvar(name_str, ty);
    fn->tok = ty->name;
    fn->is_function = true;
    fn->is_definition = EQUAL(tok, "{");
    fn->is_static = attr->is_static || (attr->is_inline && !attr->is_extern);
    fn->is_inline = attr->is_inline;
  }
  fn->align = MAX(fn->align, attr->align);
  fn->is_weak |= attr->is_weak;
  fn->section = fn->section ?: attr->section;
  fn->is_ms_abi |= attr->is_ms_abi;
  fn->visibility = fn->visibility ?: attr->visibility;
  fn->is_aligned |= attr->is_aligned;
  fn->is_noreturn |= attr->is_noreturn;
  fn->is_destructor |= attr->is_destructor;
  fn->is_constructor |= attr->is_constructor;
  fn->is_externally_visible |= attr->is_externally_visible;
  fn->is_no_instrument_function |= attr->is_no_instrument_function;
  fn->is_force_align_arg_pointer |= attr->is_force_align_arg_pointer;
  fn->is_no_caller_saved_registers |= attr->is_no_caller_saved_registers;
  fn->is_root = !(fn->is_static && fn->is_inline);
  fn->javadown = fn->javadown ?: current_javadown;
  current_javadown = NULL;
  if (consume_attribute(&tok, tok, "asm")) {
    tok = skip(tok, '(');
    fn->asmname = ConsumeStringLiteral(&tok, tok);
    tok = skip(tok, ')');
  }
  tok = attribute_list(tok, attr, thing_attributes);
  if (CONSUME(&tok, tok, ";")) return tok;
  current_fn = fn;
  locals = NULL;
  enter_scope();
  create_param_lvars(ty->params);
  // A buffer for a struct/union return value is passed
  // as the hidden first parameter.
  Type *rty = ty->return_ty;
  if ((rty->kind == TY_STRUCT || rty->kind == TY_UNION) && rty->size > 16) {
    new_lvar("", pointer_to(rty));
  }
  fn->params = locals;
  if (ty->is_variadic) {
    fn->va_area = new_lvar("__va_area__", array_of(ty_char, 136));
  }
  fn->alloca_bottom = new_lvar("__alloca_size__", pointer_to(ty_char));
  tok = skip(tok, '{');
  // [https://www.sigbus.info/n1570#6.4.2.2p1] "__func__" is
  // automatically defined as a local variable containing the
  // current function name.
  push_scope("__func__")->var =
      new_string_literal(fn->name, array_of(ty_char, strlen(fn->name) + 1));
  // [GNU] __FUNCTION__ is yet another name of __func__.
  push_scope("__FUNCTION__")->var =
      new_string_literal(fn->name, array_of(ty_char, strlen(fn->name) + 1));
  fn->body = compound_stmt(&tok, tok);
  fn->locals = locals;
  leave_scope();
  resolve_goto_labels();
  return tok;
}

static Token *global_variable(Token *tok, Type *basety, VarAttr *attr) {
  bool first = true;
  bool isjavadown = tok->kind == TK_JAVADOWN;
  while (!CONSUME(&tok, tok, ";")) {
    if (!first) tok = skip(tok, ',');
    first = false;
    Type *ty = declarator(&tok, tok, basety);
    if (!ty->name) {
      if (isjavadown) {
        return tok;
      } else {
        error_tok(ty->name_pos, "variable name omitted");
      }
    }
    Obj *var = new_gvar(get_ident(ty->name), ty);
    if (!var->tok) var->tok = ty->name;
    var->javadown = current_javadown;
    if (consume_attribute(&tok, tok, "asm")) {
      tok = skip(tok, '(');
      var->asmname = ConsumeStringLiteral(&tok, tok);
      tok = skip(tok, ')');
    }
    tok = attribute_list(tok, attr, thing_attributes);
    var->align = MAX(var->align, attr->align);
    var->is_weak = attr->is_weak;
    var->section = attr->section;
    var->visibility = attr->visibility;
    var->is_aligned = var->is_aligned | attr->is_aligned;
    var->is_externally_visible = attr->is_externally_visible;
    var->is_definition = !attr->is_extern;
    var->is_static = attr->is_static;
    var->is_tls = attr->is_tls;
    var->section = attr->section;
    if (attr->align) var->align = attr->align;
    if (EQUAL(tok, "=")) {
      gvar_initializer(&tok, tok->next, var);
    } else if (!attr->is_extern && !attr->is_tls) {
      var->is_tentative = true;
    }
  }
  return tok;
}

// Lookahead tokens and returns true if a given token is a start
// of a function definition or declaration.
static bool is_function(Token *tok) {
  if (EQUAL(tok, ";")) return false;
  Type dummy = {};
  Type *ty = declarator(&tok, tok, &dummy);
  return ty->kind == TY_FUNC;
}

// Remove redundant tentative definitions.
static void scan_globals(void) {
  Obj head;
  Obj *cur = &head;
  for (Obj *var = globals; var; var = var->next) {
    if (!var->is_tentative) {
      cur = cur->next = var;
      continue;
    }
    // Find another definition of the same identifier.
    Obj *var2 = globals;
    for (; var2; var2 = var2->next) {
      if (var != var2 && var2->is_definition &&
          !strcmp(var->name, var2->name)) {
        break;
      }
    }
    // If there's another definition, the tentative definition
    // is redundant
    if (!var2) cur = cur->next = var;
  }
  cur->next = NULL;
  globals = head.next;
}

static char *prefix_builtin(const char *name) {
  return xstrcat("__builtin_", name);
}

static Obj *declare0(char *name, Type *ret) {
  if (!opt_no_builtin) new_gvar(name, func_type(ret));
  return new_gvar(prefix_builtin(name), func_type(ret));
}

static Obj *declare1(char *name, Type *ret, Type *p1) {
  Type *ty = func_type(ret);
  ty->params = copy_type(p1);
  if (!opt_no_builtin) new_gvar(name, ty);
  return new_gvar(prefix_builtin(name), ty);
}

static Obj *declare2(char *name, Type *ret, Type *p1, Type *p2) {
  Type *ty = func_type(ret);
  ty->params = copy_type(p1);
  ty->params->next = copy_type(p2);
  if (!opt_no_builtin) new_gvar(name, ty);
  return new_gvar(prefix_builtin(name), ty);
}

static Obj *declare3(char *s, Type *r, Type *a, Type *b, Type *c) {
  Type *ty = func_type(r);
  ty->params = copy_type(a);
  ty->params->next = copy_type(b);
  ty->params->next->next = copy_type(c);
  if (!opt_no_builtin) new_gvar(s, ty);
  return new_gvar(prefix_builtin(s), ty);
}

static void math0(char *name) {
  declare0(name, ty_double);
  declare0(xstrcat(name, 'f'), ty_float);
  declare0(xstrcat(name, 'l'), ty_ldouble);
}

static void math1(char *name) {
  declare1(name, ty_double, ty_double);
  declare1(xstrcat(name, 'f'), ty_float, ty_float);
  declare1(xstrcat(name, 'l'), ty_ldouble, ty_ldouble);
}

static void math2(char *name) {
  declare2(name, ty_double, ty_double, ty_double);
  declare2(xstrcat(name, 'f'), ty_float, ty_float, ty_float);
  declare2(xstrcat(name, 'l'), ty_ldouble, ty_ldouble, ty_ldouble);
}

void declare_builtin_functions(void) {
  Type *pvoid = pointer_to(ty_void);
  Type *pchar = pointer_to(ty_char);
  builtin_alloca = declare1("alloca", pointer_to(ty_void), ty_int);
  declare0("trap", ty_int);
  declare0("ia32_pause", ty_void);
  declare0("unreachable", ty_int);
  declare1("ctz", ty_int, ty_int);
  declare1("ctzl", ty_int, ty_long);
  declare1("ctzll", ty_int, ty_long);
  declare1("clz", ty_int, ty_int);
  declare1("clzl", ty_int, ty_long);
  declare1("clzll", ty_int, ty_long);
  declare1("ffs", ty_int, ty_int);
  declare1("ffsl", ty_int, ty_long);
  declare1("ffsll", ty_int, ty_long);
  declare1("bswap16", ty_ushort, ty_ushort);
  declare1("bswap32", ty_uint, ty_uint);
  declare1("bswap64", ty_ulong, ty_ulong);
  declare1("popcount", ty_int, ty_uint);
  declare1("popcountl", ty_long, ty_ulong);
  declare1("popcountll", ty_long, ty_ulong);
  declare1("signbitf", ty_int, ty_float);
  declare1("signbit", ty_long, ty_double);
  declare1("signbitl", ty_int, ty_ldouble);
  declare1("nanf", ty_float, pchar);
  declare1("nan", ty_double, pchar);
  declare1("nanl", ty_ldouble, pchar);
  declare1("isnan", ty_int, ty_double);
  declare1("isinf", ty_int, ty_double);
  declare1("isfinite", ty_int, ty_double);
  declare2("isgreater", ty_int, ty_double, ty_double);
  declare2("isgreaterequal", ty_int, ty_double, ty_double);
  declare2("isless", ty_int, ty_double, ty_double);
  declare2("islessequal", ty_int, ty_double, ty_double);
  declare2("islessgreater", ty_int, ty_double, ty_double);
  declare2("isunordered", ty_int, ty_double, ty_double);
  declare2("strpbrk", pchar, pchar, pchar);
  declare3("memcpy", pvoid, pvoid, pvoid, ty_ulong);
  declare3("memset", pvoid, pvoid, ty_int, ty_ulong);
  declare1("strlen", ty_ulong, pchar);
  declare2("strchr", pchar, pchar, ty_int);
  declare2("strstr", pchar, pchar, pchar);
  declare1("frame_address", pvoid, ty_int);
  declare2("scalbnf", ty_float, ty_float, ty_int);
  declare2("scalbn", ty_double, ty_double, ty_int);
  declare2("scalbnl", ty_ldouble, ty_ldouble, ty_int);
  math0("inf");
  math0("huge_val");
  math1("fabs");
  math1("logb");
  math2("fmax");
  math2("fmin");
  math2("copysign");
}

// program = (typedef | function-definition | global-variable)*
Obj *parse(Token *tok) {
  declare_builtin_functions();
  globals = NULL;
  while (tok->kind != TK_EOF) {
    if (EQUAL(tok, "asm") || EQUAL(tok, "__asm__")) {
      StaticAsm *a = calloc(1, sizeof(StaticAsm));
      a->next = staticasms;
      a->body = asm_stmt(&tok, tok);
      staticasms = a;
      continue;
    }
    if (EQUAL(tok, "_Static_assert")) {
      tok = static_assertion(tok);
      continue;
    }
    if (tok->kind == TK_JAVADOWN) {
      current_javadown = tok;
      tok = tok->next;
    } else {
      current_javadown = NULL;
    }
    VarAttr attr = {};
    tok = attribute_list(tok, &attr, thing_attributes);
    Type *basety = declspec(&tok, tok, &attr);
    if (attr.is_typedef) {
      tok = parse_typedef(tok, basety);
      continue;
    }
    if (is_function(tok)) {
      tok = function(tok, basety, &attr);
      continue;
    }
    tok = global_variable(tok, basety, &attr);
  }
  for (Obj *var = globals; var; var = var->next) {
    if (var->is_root) {
      mark_live(var);
    }
  }
  scan_globals();  // remove redundant tentative definitions
  return globals;
}
