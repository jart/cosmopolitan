/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/arraylist2.internal.h"
#include "third_party/chibicc/chibicc.h"

static const char kBoolStr[2][6] = {"false", "true"};

static const char kTypeKindStr[17][8] = {
    "VOID",   "BOOL",  "CHAR",   "SHORT",   "INT",   "LONG",
    "INT128", "FLOAT", "DOUBLE", "LDOUBLE", "ENUM",  "PTR",
    "FUNC",   "ARRAY", "VLA",    "STRUCT",  "UNION",
};

static const char kNodeKindStr[50][11] = {
    "NULL_EXPR",  "ADD",     "SUB",       "MUL",       "DIV",       "NEG",
    "REM",        "BINAND",  "BINOR",     "BINXOR",    "SHL",       "SHR",
    "EQ",         "NE",      "LT",        "LE",        "ASSIGN",    "COND",
    "COMMA",      "MEMBER",  "ADDR",      "DEREF",     "NOT",       "BITNOT",
    "LOGAND",     "LOGOR",   "RETURN",    "IF",        "FOR",       "DO",
    "SWITCH",     "CASE",    "BLOCK",     "GOTO",      "GOTO_EXPR", "LABEL",
    "LABEL_VAL",  "FUNCALL", "EXPR_STMT", "STMT_EXPR", "VAR",       "VLA_PTR",
    "NUM",        "CAST",    "MEMZERO",   "ASM",       "CAS",       "EXCH",
    "FPCLASSIFY",
};

static struct Visited {
  size_t i, n;
  intptr_t *p;
} g_visited;

static void PrintObj(FILE *, int, const char *, Obj *);
static void PrintNode(FILE *, int, const char *, Node *);
static void PrintType(FILE *, int, const char *, Type *);

static bool Visit(void *ptr) {
  size_t i;
  intptr_t addr = (intptr_t)ptr;
  for (i = 0; i < g_visited.i; ++i) {
    if (addr == g_visited.p[i]) {
      return false;
    }
  }
  APPEND(&g_visited.p, &g_visited.i, &g_visited.n, &addr);
  return true;
}

static void PrintLine(FILE *f, int l, char *fmt, ...) {
  int i;
  va_list ap;
  for (i = 0; i < l; ++i) fputc(' ', f);
  va_start(ap, fmt);
  vfprintf(f, fmt, ap);
  va_end(ap);
  fputc('\n', f);
}

static void PrintBool(FILE *f, int l, const char *s, bool b) {
  if (!b) return;
  PrintLine(f, l, "%s%s", s, kBoolStr[b]);
}

static void PrintInt(FILE *f, int l, const char *s, long x) {
  if (!x) return;
  PrintLine(f, l, "%s%ld", s, x);
}

static void PrintStr(FILE *f, int l, const char *s, const char *t) {
  if (!t || !*t) return;
  PrintLine(f, l, "%s%`'s", s, t);
}

static void PrintTokStr(FILE *f, int l, const char *s, Token *t) {
  if (!t) return;
  PrintLine(f, l, "%s%`'.*s", s, t->len, t->loc);
}

static void PrintMember(FILE *f, int l, const char *s, Member *m) {
  if (!m) return;
  PrintLine(f, l, "%sMember { # %p", s, m);
  PrintTokStr(f, l + 2, "name: ", m->name);
  PrintInt(f, l + 2, "idx: ", m->idx);
  PrintInt(f, l + 2, "align: ", m->align);
  PrintInt(f, l + 2, "offset: ", m->offset);
  PrintBool(f, l + 2, "is_bitfield: ", m->is_bitfield);
  PrintInt(f, l + 2, "bit_offset: ", m->bit_offset);
  PrintInt(f, l + 2, "bit_width: ", m->bit_width);
  PrintType(f, l + 2, "ty: ", m->ty);
  PrintLine(f, l, "}");
}

static void PrintMembers(FILE *f, int l, const char *s, Member *m) {
  for (; m; m = m->next) {
    PrintMember(f, l, s, m);
  }
}

static void PrintType(FILE *f, int l, const char *s, Type *t) {
  for (; t; t = t->next) {
    if (Visit(t)) {
      PrintLine(f, l, "%sType { # %p", s, t);
      PrintLine(f, l + 2, "kind: TY_%s", kTypeKindStr[t->kind]);
      PrintInt(f, l + 2, "size: ", t->size);
      PrintInt(f, l + 2, "align: ", t->align);
      PrintBool(f, l + 2, "is_unsigned: ", t->is_unsigned);
      PrintBool(f, l + 2, "is_atomic: ", t->is_atomic);
      PrintType(f, l + 2, "base: ", t->base);
      PrintTokStr(f, l + 2, "name: ", t->name);
      PrintTokStr(f, l + 2, "name_pos: ", t->name_pos);
      PrintInt(f, l + 2, "array_len: ", t->array_len);
      PrintInt(f, l + 2, "vector_size: ", t->vector_size);
      PrintNode(f, l + 2, "vla_len: ", t->vla_len);
      PrintObj(f, l + 2, "vla_size: ", t->vla_size);
      PrintMembers(f, l + 2, "members: ", t->members);
      PrintBool(f, l + 2, "is_flexible: ", t->is_flexible);
      PrintBool(f, l + 2, "is_packed: ", t->is_packed);
      PrintBool(f, l + 2, "is_aligned: ", t->is_aligned);
      PrintBool(f, l + 2, "is_const: ", t->is_const);
      PrintBool(f, l + 2, "is_static: ", t->is_static);
      PrintType(f, l + 2, "return_ty: ", t->return_ty);
      PrintType(f, l + 2, "params: ", t->params);
      PrintBool(f, l + 2, "is_variadic: ", t->is_variadic);
      PrintLine(f, l, "}");
    } else if (t->name) {
      PrintLine(f, l, "%sTY_%s %.*s # %p", s, kTypeKindStr[t->kind],
                t->name->len, t->name->loc, t);
    } else {
      PrintLine(f, l, "%sTY_%s # %p", s, kTypeKindStr[t->kind], t);
    }
  }
}

static void PrintAsm(FILE *f, int l, const char *s, Asm *a) {
  int i;
  if (!a) return;
  PrintLine(f, l, "%sAsm { # %p", s, a);
  PrintStr(f, l + 2, "str: ", a->str);
  for (i = 0; i < a->n; ++i) {
    PrintLine(f, l + 2, "ops: AsmOperand {");
    PrintStr(f, l + 4, "str: ", a->ops[i].str);
    PrintNode(f, l + 4, "node: ", a->ops[i].node);
    PrintLine(f, l + 2, "}");
  }
  PrintLine(f, l, "}");
}

static void PrintNode(FILE *f, int l, const char *s, Node *n) {
  for (; n; n = n->next) {
    PrintLine(f, l, "%sNode { # %p", s, n);
    PrintLine(f, l + 2, "kind: ND_%s", kNodeKindStr[n->kind]);
    PrintType(f, l + 2, "ty: ", n->ty);
    PrintNode(f, l + 2, "lhs: ", n->lhs);
    PrintNode(f, l + 2, "rhs: ", n->rhs);
    PrintNode(f, l + 2, "cond: ", n->cond);
    PrintNode(f, l + 2, "then: ", n->then);
    PrintNode(f, l + 2, "els: ", n->els);
    PrintNode(f, l + 2, "init: ", n->init);
    PrintNode(f, l + 2, "inc: ", n->inc);
    PrintNode(f, l + 2, "body: ", n->body);
    PrintType(f, l + 2, "func_ty: ", n->func_ty);
    PrintNode(f, l + 2, "args: ", n->args);
    PrintObj(f, l + 2, "ret_buffer: ", n->ret_buffer);
    PrintBool(f, l + 2, "pass_by_stack: ", n->pass_by_stack);
    PrintBool(f, l + 2, "realign_stack: ", n->realign_stack);
    PrintNode(f, l + 2, "case_next: ", n->case_next);
    PrintNode(f, l + 2, "default_case: ", n->default_case);
    PrintStr(f, l + 2, "label: ", n->label);
    PrintStr(f, l + 2, "unique_label: ", n->unique_label);
    PrintNode(f, l + 2, "goto_next: ", n->goto_next);
    PrintStr(f, l + 2, "brk_label: ", n->brk_label);
    PrintStr(f, l + 2, "cont_label: ", n->cont_label);
    PrintInt(f, l + 2, "begin: ", n->begin);
    PrintAsm(f, l + 2, "azm: ", n->azm);
    PrintInt(f, l + 2, "end: ", n->end);
    PrintMember(f, l + 2, "member: ", n->member);
    PrintObj(f, l + 2, "var: ", n->var);
    PrintInt(f, l + 2, "val: ", n->val);
    if (n->fval) PrintLine(f, l + 2, "fval: %Lf", n->fval);
    PrintLine(f, l, "}");
  }
}

static void PrintRelo(FILE *f, int l, const char *s, Relocation *r) {
  for (; r; r = r->next) {
    PrintLine(f, l, "%sRelocation { # %p", s, r);
    PrintInt(f, l + 2, "offset: ", r->offset);
    if (r->label) PrintStr(f, l + 2, "label: ", *r->label);
    PrintInt(f, l + 2, "addend: ", r->addend);
    PrintLine(f, l, "}");
  }
}

static void PrintObj(FILE *f, int l, const char *s, Obj *o) {
  if (!o) return;
  PrintLine(f, l, "%sObj { # %p", s, o);
  PrintStr(f, l + 2, "name: ", o->name);
  PrintType(f, l + 2, "ty: ", o->ty);
  PrintBool(f, l + 2, "is_local: ", o->is_local);
  PrintInt(f, l + 2, "align: ", o->align);
  PrintInt(f, l + 2, "offset: ", o->offset);
  PrintBool(f, l + 2, "is_function: ", o->is_function);
  PrintBool(f, l + 2, "is_definition: ", o->is_definition);
  PrintBool(f, l + 2, "is_static: ", o->is_static);
  PrintBool(f, l + 2, "is_weak: ", o->is_weak);
  PrintBool(f, l + 2, "is_externally_visible: ", o->is_externally_visible);
  PrintStr(f, l + 2, "asmname: ", o->asmname);
  PrintStr(f, l + 2, "section: ", o->section);
  PrintStr(f, l + 2, "visibility: ", o->visibility);
  PrintBool(f, l + 2, "is_tentative: ", o->is_tentative);
  PrintBool(f, l + 2, "is_string_literal: ", o->is_string_literal);
  PrintBool(f, l + 2, "is_tls: ", o->is_tls);
  PrintStr(f, l + 2, "init_data: ", o->init_data);
  PrintRelo(f, l + 2, "rel: ", o->rel);
  PrintBool(f, l + 2, "is_inline: ", o->is_inline);
  PrintBool(f, l + 2, "is_aligned: ", o->is_aligned);
  PrintBool(f, l + 2, "is_noreturn: ", o->is_noreturn);
  PrintBool(f, l + 2, "is_destructor: ", o->is_destructor);
  PrintBool(f, l + 2, "is_constructor: ", o->is_constructor);
  PrintBool(f, l + 2, "is_externally_visible: ", o->is_externally_visible);
  PrintBool(f, l + 2,
            "is_no_instrument_function: ", o->is_no_instrument_function);
  PrintBool(f, l + 2,
            "is_force_align_arg_pointer: ", o->is_force_align_arg_pointer);
  PrintBool(f, l + 2,
            "is_no_caller_saved_registers: ", o->is_no_caller_saved_registers);
  PrintInt(f, l + 2, "stack_size: ", o->stack_size);
  PrintObj(f, l + 2, "params: ", o->params);
  PrintNode(f, l + 2, "body: ", o->body);
  PrintObj(f, l + 2, "locals: ", o->locals);
  PrintObj(f, l + 2, "va_area: ", o->va_area);
  PrintObj(f, l + 2, "alloca_bottom: ", o->alloca_bottom);
  PrintBool(f, l + 2, "is_live: ", o->is_live);
  PrintBool(f, l + 2, "is_root: ", o->is_root);
  PrintLine(f, l, "}");
}

void print_ast(FILE *f, Obj *o) {
  for (; o; o = o->next) {
    PrintObj(f, 0, "", o);
  }
}
