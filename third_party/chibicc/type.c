#include "libc/assert.h"
#include "third_party/chibicc/chibicc.h"

/* TODO(jart): Why can't these be const? */
Type ty_void[1] = {{TY_VOID, 1, 1}};
Type ty_bool[1] = {{TY_BOOL, 1, 1}};
Type ty_char[1] = {{TY_CHAR, 1, 1}};
Type ty_short[1] = {{TY_SHORT, 2, 2}};
Type ty_int[1] = {{TY_INT, 4, 4}};
Type ty_long[1] = {{TY_LONG, 8, 8}};
Type ty_int128[1] = {{TY_INT128, 16, 16}};
Type ty_uchar[1] = {{TY_CHAR, 1, 1, true}};
Type ty_ushort[1] = {{TY_SHORT, 2, 2, true}};
Type ty_uint[1] = {{TY_INT, 4, 4, true}};
Type ty_ulong[1] = {{TY_LONG, 8, 8, true}};
Type ty_uint128[1] = {{TY_INT128, 16, 16, true}};
Type ty_float[1] = {{TY_FLOAT, 4, 4}};
Type ty_double[1] = {{TY_DOUBLE, 8, 8}};
Type ty_ldouble[1] = {{TY_LDOUBLE, 16, 16}};

static Type *new_type(TypeKind kind, int size, int align) {
  Type *ty = alloc_type();
  ty->kind = kind;
  ty->size = size;
  ty->align = align;
  return ty;
}

bool is_integer(Type *ty) {
  TypeKind k = ty->kind;
  return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT || k == TY_INT ||
         k == TY_LONG || k == TY_INT128 || k == TY_ENUM;
}

bool is_flonum(Type *ty) {
  return ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE ||
         ty->kind == TY_LDOUBLE;
}

bool is_numeric(Type *ty) {
  return is_integer(ty) || is_flonum(ty);
}

bool is_compatible(Type *t1, Type *t2) {
  if (t1 == t2) return true;
  if (t1->origin) return is_compatible(t1->origin, t2);
  if (t2->origin) return is_compatible(t1, t2->origin);
  if (t1->kind != t2->kind) return false;
  switch (t1->kind) {
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
    case TY_LONG:
    case TY_INT128:
      return t1->is_unsigned == t2->is_unsigned;
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
      return true;
    case TY_PTR:
      return is_compatible(t1->base, t2->base);
    case TY_FUNC: {
      if (!is_compatible(t1->return_ty, t2->return_ty)) return false;
      if (t1->is_variadic != t2->is_variadic) return false;
      Type *p1 = t1->params;
      Type *p2 = t2->params;
      for (; p1 && p2; p1 = p1->next, p2 = p2->next) {
        if (!is_compatible(p1, p2)) return false;
      }
      return p1 == NULL && p2 == NULL;
    }
    case TY_ARRAY:
      if (!is_compatible(t1->base, t2->base)) return false;
      return t1->array_len < 0 && t2->array_len < 0 &&
             t1->array_len == t2->array_len;
  }
  return false;
}

Type *copy_type(Type *ty) {
  Type *ret = alloc_type();
  *ret = *ty;
  ret->origin = ty;
  return ret;
}

Type *pointer_to(Type *base) {
  Type *ty = new_type(TY_PTR, 8, 8);
  ty->base = base;
  ty->is_unsigned = true;
  return ty;
}

Type *func_type(Type *return_ty) {
  // The C spec disallows sizeof(<function type>), but
  // GCC allows that and the expression is evaluated to 1.
  Type *ty = new_type(TY_FUNC, 1, 1);
  ty->return_ty = return_ty;
  return ty;
}

Type *array_of(Type *base, int len) {
  Type *ty = new_type(TY_ARRAY, base->size * len, base->align);
  ty->base = base;
  ty->array_len = len;
  return ty;
}

Type *vla_of(Type *base, Node *len) {
  Type *ty = new_type(TY_VLA, 8, 8);
  ty->base = base;
  ty->vla_len = len;
  return ty;
}

Type *enum_type(void) {
  Type *ty = new_type(TY_ENUM, 4, 4);
  ty->is_unsigned = true;
  return ty;
}

Type *struct_type(void) {
  return new_type(TY_STRUCT, 0, 1);
}

static Type *get_common_type(Type *ty1, Type *ty2) {
  if (ty1->base) return pointer_to(ty1->base);
  if (ty1->kind == TY_FUNC) return pointer_to(ty1);
  if (ty2->kind == TY_FUNC) return pointer_to(ty2);
  if (ty1->kind == TY_LDOUBLE || ty2->kind == TY_LDOUBLE) return ty_ldouble;
  if (ty1->kind == TY_DOUBLE || ty2->kind == TY_DOUBLE) return ty_double;
  if (ty1->kind == TY_FLOAT || ty2->kind == TY_FLOAT) return ty_float;
  if (ty1->size < 4) ty1 = ty_int;
  if (ty2->size < 4) ty2 = ty_int;
  if (ty1->size != ty2->size) return (ty1->size < ty2->size) ? ty2 : ty1;
  if (ty2->is_unsigned) return ty2;
  return ty1;
}

// For many binary operators, we implicitly promote operands so that
// both operands have the same type. Any integral type smaller than
// int is always promoted to int. If the type of one operand is larger
// than the other's (e.g. "long" vs. "int"), the smaller operand will
// be promoted to match with the other.
//
// This operation is called the "usual arithmetic conversion".
static void usual_arith_conv(Node **lhs, Node **rhs) {
  if (!(*lhs)->ty || !(*rhs)->ty) {
    error_tok((*lhs)->tok, "internal npe error");
  }
  Type *ty = get_common_type((*lhs)->ty, (*rhs)->ty);
  *lhs = new_cast(*lhs, ty);
  *rhs = new_cast(*rhs, ty);
}

void add_type(Node *node) {
  if (!node || node->ty) return;
  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);
  for (Node *n = node->body; n; n = n->next) add_type(n);
  for (Node *n = node->args; n; n = n->next) add_type(n);
  switch (node->kind) {
    case ND_NUM:
      node->ty = ty_int;
      return;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_REM:
    case ND_BINAND:
    case ND_BINOR:
    case ND_BINXOR:
      usual_arith_conv(&node->lhs, &node->rhs);
      node->ty = node->lhs->ty;
      return;
    case ND_NEG: {
      Type *ty = get_common_type(ty_int, node->lhs->ty);
      node->lhs = new_cast(node->lhs, ty);
      node->ty = ty;
      return;
    }
    case ND_ASSIGN:
      if (node->lhs->ty->kind == TY_ARRAY)
        error_tok(node->lhs->tok, "not an lvalue!");
      if (node->lhs->ty->kind != TY_STRUCT)
        node->rhs = new_cast(node->rhs, node->lhs->ty);
      node->ty = node->lhs->ty;
      return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
      usual_arith_conv(&node->lhs, &node->rhs);
      node->ty = ty_int;
      return;
    case ND_FUNCALL:
      node->ty = node->func_ty->return_ty;
      return;
    case ND_NOT:
    case ND_LOGOR:
    case ND_LOGAND:
      node->ty = ty_int;
      return;
    case ND_BITNOT:
    case ND_SHL:
    case ND_SHR:
      node->ty = node->lhs->ty;
      return;
    case ND_VAR:
    case ND_VLA_PTR:
      node->ty = node->var->ty;
      return;
    case ND_COND:
      if (node->then->ty->kind == TY_VOID || node->els->ty->kind == TY_VOID) {
        node->ty = ty_void;
      } else {
        usual_arith_conv(&node->then, &node->els);
        node->ty = node->then->ty;
      }
      return;
    case ND_COMMA:
      node->ty = node->rhs->ty;
      return;
    case ND_MEMBER:
      node->ty = node->member->ty;
      return;
    case ND_ADDR: {
      Type *ty = node->lhs->ty;
      if (ty->kind == TY_ARRAY) {
        node->ty = pointer_to(ty->base);
      } else {
        node->ty = pointer_to(ty);
      }
      return;
    }
    case ND_DEREF:
#if 0
      if (node->lhs->ty->size == 16 && (node->lhs->ty->kind == TY_FLOAT ||
                                        node->lhs->ty->kind == TY_DOUBLE)) {
        node->ty = node->lhs->ty;
      } else {
#endif
      if (!node->lhs->ty->base) {
        error_tok(node->tok, "invalid pointer dereference");
      }
      if (node->lhs->ty->base->kind == TY_VOID) {
        /* TODO(jart): Does standard permit this? */
        /* https://lkml.org/lkml/2018/3/20/845    */
        error_tok(node->tok, "dereferencing a void pointer");
      }
      node->ty = node->lhs->ty->base;
#if 0
      }
#endif
      return;
    case ND_STMT_EXPR:
      if (node->body) {
        Node *stmt = node->body;
        for (;;) {
          if (stmt->next) {
            stmt = stmt->next;
          } else {
            if (stmt->kind == ND_LABEL && stmt->lhs) {
              stmt = stmt->lhs;
            } else {
              break;
            }
          }
        }
        if (stmt->kind == ND_EXPR_STMT) {
          node->ty = stmt->lhs->ty;
          return;
        }
      }
      error_tok(node->tok,
                "statement expression returning void is not supported");
      return;
    case ND_LABEL_VAL:
      node->ty = pointer_to(ty_void);
      return;
    case ND_CAS:
      add_type(node->cas_addr);
      add_type(node->cas_old);
      add_type(node->cas_new);
      node->ty = ty_bool;
      if (node->cas_addr->ty->kind != TY_PTR)
        error_tok(node->cas_addr->tok, "pointer expected");
      if (node->cas_old->ty->kind != TY_PTR)
        error_tok(node->cas_old->tok, "pointer expected");
      return;
    case ND_EXCH_N:
    case ND_FETCHADD:
    case ND_FETCHSUB:
    case ND_FETCHXOR:
    case ND_FETCHAND:
    case ND_FETCHOR:
    case ND_SUBFETCH:
      if (node->lhs->ty->kind != TY_PTR)
        error_tok(node->lhs->tok, "pointer expected");
      node->rhs = new_cast(node->rhs, node->lhs->ty->base);
      node->ty = node->lhs->ty->base;
      return;
  }
}
