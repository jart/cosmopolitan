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
#include "libc/intrin/bsf.h"
#include "libc/intrin/bsr.h"
#include "third_party/chibicc/chibicc.h"

#define PRECIOUS 0b1111000000101000  // bx,bp,r12-r15

StaticAsm *staticasms;

static const char kGreg[4][16][5] = {
    {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b",
     "r11b", "r12b", "r13b", "r14b", "r15b"},
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "r8w", "r9w", "r10w",
     "r11w", "r12w", "r13w", "r14w", "r15w"},
    {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d",
     "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"},
    {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10",
     "r11", "r12", "r13", "r14", "r15"},
};

static void DecodeAsmConstraints(AsmOperand *op) {
  int i;
  char c;
  for (i = 0;;) {
    switch ((c = op->str[i++])) {
      case '\0':
      case ',':  // alternative group
        return;  // todo: read combos
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':  // reference
      case '=':  // output
      case '+':  // output and input
        op->flow = c;
        break;
      case 'm':  // memory
      case 'o':  // memory offsetable
        op->type |= kAsmMem;
        op->regmask |= 0b1111111111111111;
        break;
      case 'i':  // int literal, c/asm constexpr, ld embedding
      case 'n':  // integer literal or compiler constexpr?
      case 's':  // integer constexpr but not literal (or known at link time?)
      case 'M':  // i∊[0,3] for index scaling, e.g. "mov\t(?,?,1<<%0),?"
      case 'I':  // i∊[0,31] 5 bits for 32-bit shifts
      case 'J':  // i∊[0,63] 6 bits for 64-bit shifts
      case 'N':  // i∊[0,255] in/out immediate byte
      case 'K':  // i∊[-128,127] signed byte integer
      case 'e':  // i∊[-2^31,2^31) for sign-extending
      case 'Z':  // i∊[0,2³²) for zero-extending
      case 'L':  // i∊{0xFF,0xFFFF,0xFFFFFFFF}
        op->type |= kAsmImm;
        break;
      case 'a':  // ax
        op->regmask |= 0b0000000000000001;
        op->type |= kAsmReg;
        break;
      case 'c':  // cx
        op->regmask |= 0b0000000000000010;
        op->type |= kAsmReg;
        break;
      case 'd':  // dx
        op->regmask |= 0b0000000000000100;
        op->type |= kAsmReg;
        break;
      case 'b':  // bx
        op->regmask |= 0b0000000000001000;
        op->type |= kAsmReg;
        break;
      case 'S':  // si
        op->regmask |= 0b0000000001000000;
        op->type |= kAsmReg;
        break;
      case 'D':  // di
        op->regmask |= 0b0000000010000000;
        op->type |= kAsmReg;
        break;
      case 'r':  // general register
        op->regmask |= 0b1111111111111111;
        op->type |= kAsmReg;
        break;
      case 'q':  // greg lo-byte accessible
        op->regmask |= 0b1111111111111111;
        op->type |= kAsmReg;
        break;
      case 'Q':  // greg hi-byte accessible
        op->regmask |= 0b0000000000001111;
        op->type |= kAsmReg;
        break;
      case 'U':  // greg call-clobbered
        op->regmask |= 0b0000111111000111;
        op->type |= kAsmReg;
        break;
      case 'R':  // greg all models
        op->regmask |= 0b0000000011111111;
        op->type |= kAsmReg;
        break;
      case 'l':  // index register
        op->regmask |= 0b1111111111101111;
        op->type |= kAsmReg;
        break;
      case 'y':  // mmx
        op->type |= kAsmMmx;
        op->regmask |= 0b0000000011111111;
        break;
      case 'x':  // xmm
        op->type |= kAsmXmm;
        op->regmask |= 0b1111111111111111;
        break;
      case 'g':  // rmi
        op->type |= kAsmImm | kAsmMem | kAsmReg;
        op->regmask |= 0b1111111111111111;
        break;
      case 'X':  // anything
        op->type |= kAsmImm | kAsmMem | kAsmReg | kAsmXmm | kAsmFpu | kAsmRaw;
        op->regmask |= 0b1111111111111111;
        op->x87mask |= 0b11111111;
        break;
      case 't':  // %st
        op->type |= kAsmFpu;
        op->x87mask |= 0b00000001;
        break;
      case 'u':  // %st(1)
        op->type |= kAsmFpu;
        op->x87mask |= 0b00000010;
        break;
      case 'f':  // %st(0..7)
        op->type |= kAsmFpu;
        op->x87mask |= 0b11111111;
        break;
      case 'A':  // ax+dx
        error_tok(op->tok, "ax dx constraint not implemented");
      case '@':  // flags
        if (op->flow != '=' || strlen(op->str + i) < 3 ||
            (op->str[i] != 'c' || op->str[i + 1] != 'c')) {
          error_tok(op->tok, "invalid flag constraint");
        }
        if (!is_integer(op->node->ty) || op->node->ty->size != 1) {
          error_tok(op->node->tok, "invalid output flag type");
        }
        op->type = kAsmFlag;
        op->predicate = i + 2;
        return;
      default:
        break;
    }
  }
}

static bool IsLvalue(AsmOperand *op) {
  switch (op->node->kind) {
    case ND_VAR:
    case ND_DEREF:
    case ND_MEMBER:
    case ND_VLA_PTR:
      return true;
    default:
      return false;
  }
}

static bool CanUseReg(Node **n) {
  if ((*n)->ty->kind == TY_ARRAY) {
    *n = new_cast(*n, pointer_to((*n)->ty->base));
    return true;
  }
  return is_integer((*n)->ty) || (*n)->ty->kind == TY_PTR;
}

static bool CanUseXmm(Node *n) {
  return n->ty->vector_size == 16 || n->ty->kind == TY_FLOAT ||
         n->ty->kind == TY_DOUBLE || n->ty->kind == TY_PTR ||
         (n->ty->kind == TY_ARRAY && n->ty->size == 16);
}

static bool CanUseMmx(Node *n) {
  return n->ty->kind == TY_FLOAT || n->ty->kind == TY_DOUBLE ||
         n->ty->kind == TY_PTR || (n->ty->kind == TY_ARRAY && n->ty->size == 8);
}

static int PickAsmReferenceType(AsmOperand *op, AsmOperand *ref) {
  switch (ref->type) {
    case kAsmImm:
    case kAsmMem:
      error_tok(op->tok, "bad reference");
    case kAsmReg:
      if (!CanUseReg(&op->node)) {
        error_tok(op->tok, "expected integral expression");
      }
      op->regmask = 0;
      return ref->type;
    case kAsmXmm:
      if (!CanUseXmm(op->node)) {
        error_tok(op->tok, "expected xmm expression");
      }
      return ref->type;
    case kAsmFpu:
      if (op->node->ty->kind != TY_LDOUBLE) {
        error_tok(op->tok, "expected long double expression");
      }
      op->x87mask = 0;
      return ref->type;
    default:
      UNREACHABLE();
  }
}

static int PickAsmOperandType(Asm *a, AsmOperand *op) {
  if (op->flow == '=' || op->flow == '+') {
    op->type &= ~kAsmImm;
    if (!IsLvalue(op)) error_tok(op->tok, "lvalue required");
  }
  if ((op->type & kAsmImm) && (is_const_expr(op->node) ||
                               // TODO(jart): confirm this is what we want
                               op->node->ty->kind == TY_FUNC)) {
    op->val = eval2(op->node, &op->label);
    return kAsmImm;
  }
  if ((op->type & kAsmMem) && op->node->ty->kind != TY_VOID) return kAsmMem;
  if ((op->type & kAsmFpu) && op->node->ty->kind == TY_LDOUBLE) return kAsmFpu;
  if ((op->type & kAsmXmm) && CanUseXmm(op->node)) return kAsmXmm;
  if ((op->type & kAsmMmx) && CanUseMmx(op->node)) return kAsmMmx;
  if ((op->type & kAsmReg) && CanUseReg(&op->node)) return kAsmReg;
  if (op->type & kAsmFlag) return kAsmFlag;
  if (op->type & kAsmRaw) return kAsmRaw;
  error_tok(op->tok, "constraint mismatch");
}

static Token *ParseAsmOperand(Asm *a, AsmOperand *op, Token *tok) {
  int i;
  op->tok = tok;
  op->str = ConsumeStringLiteral(&tok, tok);
  tok = skip(tok, '(');
  op->node = expr(&tok, tok);
  add_type(op->node);
  DecodeAsmConstraints(op);
  if (isdigit(op->flow)) {
    if ((i = op->flow - '0') >= a->n) error_tok(op->tok, "bad reference");
    op->type = PickAsmReferenceType(op, a->ops + i);
  } else {
    op->type = PickAsmOperandType(a, op);
  }
  return skip(tok, ')');
}

static Token *ParseAsmOperands(Asm *a, Token *tok) {
  if (EQUAL(tok, ":")) return tok;
  for (;;) {
    if (a->n == ARRAYLEN(a->ops)) {
      error_tok(tok, "too many asm operands");
    }
    tok = ParseAsmOperand(a, &a->ops[a->n], tok);
    ++a->n;
    if (!EQUAL(tok, ",")) break;
    tok = skip(tok, ',');
  }
  return tok;
}

static void CouldNotAllocateRegister(AsmOperand *op, const char *kind) {
  error_tok(op->tok, "could not allocate %s register", kind);
}

static void PickAsmRegisters(Asm *a) {
  int i, j, m, pick, regset, xmmset, x87sts;
  regset = 0b1111111111001111;  // exclude bx,sp,bp
  xmmset = 0b1111111111111111;
  x87sts = 0b0000000011111111;
  regset ^= regset & a->regclob;  // don't allocate from clobber list
  xmmset ^= xmmset & a->xmmclob;
  x87sts ^= x87sts & a->x87clob;
  for (j = 1; j <= 16; ++j) {  // iterate from most to least restrictive
    for (i = 0; i < a->n; ++i) {
      switch (a->ops[i].type) {
        case kAsmMem:
        case kAsmReg:
          if (!(m = a->ops[i].regmask)) break;
          if (popcnt(m) != j) break;
          if (!(m &= regset)) CouldNotAllocateRegister(&a->ops[i], "rm");
          pick = 1 << (a->ops[i].reg = _bsf(m));
          if (pick & PRECIOUS) a->regclob |= pick;
          regset &= ~pick;
          a->ops[i].regmask = 0;
          break;
        case kAsmXmm:
          if (!(m = a->ops[i].regmask)) break;
          if (!(m &= xmmset)) CouldNotAllocateRegister(&a->ops[i], "xmm");
          xmmset &= ~(1 << (a->ops[i].reg = _bsf(m)));
          a->ops[i].regmask = 0;
          break;
        case kAsmFpu:
          if (!(m = a->ops[i].x87mask)) break;
          if (popcnt(m) != j) break;
          if (!(m &= x87sts)) CouldNotAllocateRegister(&a->ops[i], "fpu");
          x87sts &= ~(1 << (a->ops[i].reg = _bsf(m)));
          a->ops[i].x87mask = 0;
          break;
        default:
          a->ops[i].regmask = 0;
          a->ops[i].x87mask = 0;
          break;
      }
    }
  }
}

static void MarkUsedAsmOperands(Asm *a) {
  char c, *p;
  if (!a->isgnu) return;
  for (p = a->str; (c = *p++);) {
    if (c == '%') {
      if (!(c = *p++)) error_tok(a->tok, "unexpected nul");
      if (c == '%') continue;
      if (!isdigit(c)) {
        if (!(c = *p++)) error_tok(a->tok, "unexpected nul");
        if (!isdigit(c)) {
          error_tok(a->tok, "bad asm specifier");
        }
      }
      a->ops[c - '0'].isused = true;
    }
  }
}

static int GetIndexOfRegisterName(const char *s) {
  int i, j;
  for (i = 0; i < 16; ++i) {
    for (j = 0; j < 4; ++j) {
      if (!strcmp(s, kGreg[j][i])) {
        return i;
      }
    }
  }
  return -1;
}

static Token *ParseAsmClobbers(Asm *a, Token *tok) {
  int i;
  char *s;
  Token *stok;
  for (;;) {
    stok = tok;
    s = ConsumeStringLiteral(&tok, tok);
    if (*s == '%') ++s;
    if (!strcmp(s, "cc")) {
      a->flagclob = true;
    } else if ((i = GetIndexOfRegisterName(s)) != -1) {
      a->regclob |= 1 << i;
    } else if (startswith(s, "xmm") && isdigit(s[3]) &&
               (!s[4] || isdigit(s[4]))) {
      i = s[3] - '0';
      if (s[4]) {
        i *= 10;
        i += s[4] - '0';
      }
      i &= 15;
      a->xmmclob |= 1 << i;
    } else if (!strcmp(s, "st")) {
      a->x87clob |= 1;
    } else if (startswith(s, "st(") && isdigit(s[3]) && s[4] == ')') {
      i = s[3] - '0';
      i &= 7;
      a->x87clob |= 1 << i;
    } else if (!strcmp(s, "memory")) {
      /* do nothing */
    } else {
      error_tok(stok, "unknown clobber register");
    }
    if (!EQUAL(tok, ",")) break;
    tok = skip(tok, ',');
  }
  return tok;
}

// parses ansi c11 asm statement officially defined as follows
//
//   asm-stmt = "asm" ("volatile" | "inline")* "(" string-literal ")"
//
// gnu c defines a notation for inputs, outputs, and clobbers, e.g.
//
//   asm("foo %1,%0"
//       : "=r"(x)
//       : "r"(x)
//       : "cc");
//
Asm *asm_stmt(Token **rest, Token *tok) {
  Asm *a = calloc(1, sizeof(Asm));
  tok = tok->next;
  while (EQUAL(tok, "volatile") || EQUAL(tok, "inline")) tok = tok->next;
  tok = skip(tok, '(');
  a->tok = tok;
  a->str = ConsumeStringLiteral(&tok, tok);
  if (!EQUAL(tok, ")")) {
    a->isgnu = true;
    tok = skip(tok, ':');
    tok = ParseAsmOperands(a, tok);
    if (!EQUAL(tok, ")")) {
      tok = skip(tok, ':');
      tok = ParseAsmOperands(a, tok);
      if (!EQUAL(tok, ")")) {
        tok = skip(tok, ':');
        tok = ParseAsmClobbers(a, tok);
      }
    }
  }
  PickAsmRegisters(a);
  MarkUsedAsmOperands(a);
  *rest = skip(tok, ')');
  return a;
}

static void PrintAsmConstant(AsmOperand *op) {
  if (op->label) {
    fprintf(output_stream, "%s%+ld", *op->label, op->val);
  } else {
    fprintf(output_stream, "%ld", op->val);
  }
}

static void EmitAsmSpecifier(AsmOperand *op, int q, int z) {
  if (!q) {
    switch (op->type) {
      case kAsmImm:
        fputc('$', output_stream);
        PrintAsmConstant(op);
        break;
      case kAsmMem:
        fprintf(output_stream, "(%%%s)", kGreg[3][op->reg]);
        break;
      case kAsmReg:
        fprintf(output_stream, "%%%s", kGreg[z][op->reg]);
        break;
      case kAsmXmm:
        fprintf(output_stream, "%%xmm%d", op->reg);
        break;
      case kAsmFpu:
        fprintf(output_stream, "%%st(%d)", op->reg);
        break;
      case kAsmRaw:
        fprintf(output_stream, "%.*s", op->node->tok->len, op->node->tok->loc);
        break;
      default:
        UNREACHABLE();
    }
  } else {
    switch (q) {
      case 'h':  // hi byte
        fprintf(output_stream, "%%%ch", "acdb"[op->reg]);
        break;
      case 'b':  // lo byte
        fprintf(output_stream, "%%%s", kGreg[0][op->reg]);
        break;
      case 'w':  // word
        fprintf(output_stream, "%%%s", kGreg[1][op->reg]);
        break;
      case 'k':  // dword
        fprintf(output_stream, "%%%s", kGreg[2][op->reg]);
        break;
      case 'q':  // qword
        fprintf(output_stream, "%%%s", kGreg[3][op->reg]);
        break;
      case 'z':  // print suffix
        fprintf(output_stream, "%c", "bwlq"[z]);
        break;
      case 'p':  // print raw
        fprintf(output_stream, "%.*s", op->node->tok->len, op->node->tok->loc);
        break;
      case 'a':  // print address
        PrintAsmConstant(op);
        break;
      case 'c':  // print constant w/o punctuation
        PrintAsmConstant(op);
        break;
      case 'P':  // print w/ @plt
        PrintAsmConstant(op);
        fprintf(output_stream, "@plt");
        break;
      case 'l':  // print label w/o punctuation
        if (!op->label) {
          error_tok(op->tok, "qualifier expected label");
        }
        fprintf(output_stream, "%s", *op->label);
        break;
      case 'V':  // print register w/o punctuation
        if (op->type != kAsmReg) {
          error_tok(op->tok, "qualifier expected register");
        }
        fprintf(output_stream, "%s", kGreg[z][op->reg]);
        break;
      default:
        error_tok(op->tok, "bad asm qualifier %%%`'c", q);
    }
  }
}

static char *HandleAsmSpecifier(Asm *a, char *p) {
  int c, i, q, z;
  if (!(c = *p++)) error_tok(a->tok, "unexpected nul");
  if (c == '%') {
    fputc('%', output_stream);
    return p;
  }
  if (c == '=') {
    fprintf(output_stream, "%d", count());
    return p;
  }
  if (isdigit(c)) {
    q = '\0';
  } else {
    q = c;
    if (!(c = *p++)) error_tok(a->tok, "unexpected nul");
    if (!isdigit(c)) {
      error_tok(a->tok, "bad asm specifier at offset %d", p - a->str);
    }
  }
  if ((i = c - '0') >= a->n) {
    error_tok(a->tok, "bad asm reference at offset %d", p - a->str);
  }
  z = _bsr(a->ops[i].node->ty->size);
  if (z > 3 && a->ops[i].type == kAsmReg) {
    error_tok(a->tok, "bad asm op size");
  }
  EmitAsmSpecifier(&a->ops[i], q, z);
  return p;
}

static void EmitAsmText(Asm *a) {
  char c, *p;
  if (*a->str) {
    if (a->isgnu) {
      flushln();
      fprintf(output_stream, "\t");
      for (p = a->str;;) {
        switch ((c = *p++)) {
          case '\0':
            fputc('\n', output_stream);
            return;
          case '%':
            p = HandleAsmSpecifier(a, p);
            break;
          default:
            fputc(c, output_stream);
            break;
        }
      }
    } else {
      println("\t%s", a->str);
    }
  }
}

static void PushAsmInput(AsmOperand *op) {
  gen_expr(op->node);
  push();
}

static void PushAsmInputs(Asm *a) {
  int i;
  for (i = 0; i < a->n; ++i) {
    if (a->ops[i].flow == '=') continue;
    switch (a->ops[i].type) {
      case kAsmReg:
        PushAsmInput(&a->ops[i]);
        break;
      case kAsmMem:
        if (a->ops[i].isused) {
          PushAsmInput(&a->ops[i]);
        }
        break;
      case kAsmXmm:
        gen_expr(a->ops[i].node);
        println("\tsub\t$16,%%rsp");
        switch (a->ops[i].node->ty->kind) {
          case TY_FLOAT:
          case TY_DOUBLE:
            println("\tmovdqu\t%%xmm0,(%%rsp)");
            break;
          default:
            println("\tmovdqu\t(%%rax),%%xmm0");
            println("\tmovdqu\t%%xmm0,(%%rsp)");
            break;
        }
        break;
      case kAsmMmx:
        gen_expr(a->ops[i].node);
        println("\tsub\t$8,%%rsp");
        switch (a->ops[i].node->ty->kind) {
          case TY_FLOAT:
          case TY_DOUBLE:
            println("\tmovq\t%%mm0,(%%rsp)");
            break;
          default:
            println("\tmovq\t(%%rax),%%mm0");
            println("\tmovq\t%%mm0,(%%rsp)");
            break;
        }
        break;
      case kAsmFpu: /* TODO: How does this work in non-simple case? */
        gen_expr(a->ops[i].node);
        println("\tsub\t$16,%%rsp");
        println("\tfstpt\t(%%rsp)");
        break;
      default:
        break;
    }
  }
}

static void PopAsmInput(Asm *a, AsmOperand *op) {
  if (isdigit(op->flow)) {
    popreg(kGreg[3][a->ops[op->flow - '0'].reg]);
  } else {
    popreg(kGreg[3][op->reg]);
  }
}

static void PopAsmInputs(Asm *a) {
  int i;
  for (i = a->n; i--;) {
    if (a->ops[i].flow == '=') continue;
    switch (a->ops[i].type) {
      case kAsmReg:
        PopAsmInput(a, &a->ops[i]);
        break;
      case kAsmMem:
        if (a->ops[i].isused) {
          PopAsmInput(a, &a->ops[i]);
        }
        break;
      case kAsmXmm:
        println("\tmovdqu\t(%%rsp),%%xmm%d", a->ops[i].reg);
        println("\tadd\t$16,%%rsp");
        break;
      case kAsmMmx:
        println("\tmovq\t(%%rsp),%%mm%d", a->ops[i].reg);
        println("\tadd\t$8,%%rsp");
        break;
      case kAsmFpu: /* TODO: How does this work in non-simple case? */
        println("\tfldt\t(%%rsp)");
        println("\tadd\t$16,%%rsp");
        break;
      default:
        break;
    }
  }
}

static void StoreAsmOutputs(Asm *a) {
  int i, z;
  for (i = 0; i < a->n; ++i) {
    if (a->ops[i].flow == '=' || a->ops[i].flow == '+') {
      switch (a->ops[i].type) {
        case kAsmFlag:
          gen_addr(a->ops[i].node);
          println("\tset%s\t(%%rax)", a->ops[i].str + a->ops[i].predicate);
          break;
        case kAsmReg:
          z = _bsr(a->ops[i].node->ty->size);
          if (a->ops[i].reg) {
            gen_addr(a->ops[i].node);
            if (z > 3) error_tok(a->tok, "bad asm out size");
            println("\tmov\t%%%s,(%%rax)", kGreg[z][a->ops[i].reg]);
          } else {
            println("\tpush\t%%rbx");
            println("\tmov\t%%rax,%%rbx");
            gen_addr(a->ops[i].node);
            println("\tmov\t%%%s,(%%rax)", kGreg[z][3]);
            println("\tpop\t%%rbx");
          }
          break;
        case kAsmXmm:
          gen_addr(a->ops[i].node);
          switch (a->ops[i].node->ty->kind) {
            case TY_FLOAT:
              println("\tmovss\t%%xmm%d,(%%rax)", a->ops[i].reg);
              break;
            case TY_DOUBLE:
              println("\tmovsd\t%%xmm%d,(%%rax)", a->ops[i].reg);
              break;
            default:
              println("\tmovdqu\t%%xmm%d,(%%rax)", a->ops[i].reg);
              break;
          }
          break;
        case kAsmMmx:
          gen_addr(a->ops[i].node);
          switch (a->ops[i].node->ty->kind) {
            case TY_FLOAT:
              println("\tmovss\t%%mm%d,(%%rax)", a->ops[i].reg);
              break;
            case TY_DOUBLE:
              println("\tmovsd\t%%mm%d,(%%rax)", a->ops[i].reg);
              break;
            default:
              println("\tmovq\t%%mm%d,(%%rax)", a->ops[i].reg);
              break;
          }
          break;
        case kAsmFpu: /* TODO: How does this work in non-simple case? */
          gen_addr(a->ops[i].node);
          println("\tfstpt\t(%%rax)");
          break;
        default:
          break;
      }
    }
  }
}

static void PushClobbers(Asm *a) {
  int i, regs = a->regclob & PRECIOUS;
  while (regs) {
    i = _bsf(regs);
    pushreg(kGreg[3][i]);
    regs &= ~(1 << i);
  }
}

static void PopClobbers(Asm *a) {
  int i, regs = a->regclob & PRECIOUS;
  while (regs) {
    i = _bsr(regs);
    popreg(kGreg[3][i]);
    regs &= ~(1 << i);
  }
}

// generates shocking horrible code for parsed asm statement
void gen_asm(Asm *a) {
  PushAsmInputs(a);
  print_loc(a->tok->file->file_no, a->tok->line_no);
  PopAsmInputs(a);
  PushClobbers(a);
  EmitAsmText(a);
  StoreAsmOutputs(a);
  PopClobbers(a);
}
