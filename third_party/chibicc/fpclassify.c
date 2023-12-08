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
#include "third_party/chibicc/chibicc.h"

/**
 * @fileoverview __builtin_fpclassify() implementation
 */

#define FPCLASSIFY_FLOAT \
  "\tmovaps\t%%xmm0,%%xmm1\n\
\tmov\t$0x7fffffff,%%eax\n\
\tmovd\t%%eax,%%xmm2\n\
\tandps\t%%xmm2,%%xmm1\n\
\tmov\t$%d,%%eax\n\
\tucomiss\t%%xmm1,%%xmm1\n\
\tjp\t9f\n\
\tmov\t$0x7f7fffff,%%edi\n\
\tmovd\t%%edi,%%xmm2\n\
\tucomiss\t%%xmm2,%%xmm1\n\
\tja\t2f\n\
\tmov\t$0x00800000,%%edi\n\
\tmovd\t%%edi,%%xmm2\n\
\tucomiss\t%%xmm2,%%xmm1\n\
\tjnb\t3f\n\
\txorps\t%%xmm1,%%xmm1\n\
\tucomiss\t%%xmm1,%%xmm0\n\
\tjp\t1f\n\
\tmovl\t$%d,%%eax\n\
\tje\t9f\n\
1:\tmovl\t$%d,%%eax\n\
\tjmp\t9f\n\
2:\tmovl\t$%d,%%eax\n\
\tjmp\t9f\n\
3:\tmovl\t$%d,%%eax\n\
9:"

#define FPCLASSIFY_DOUBLE \
  "\tmovapd\t%%xmm0,%%xmm1\n\
\tmov\t$0x7fffffffffffffff,%%rax\n\
\tmovq\t%%rax,%%xmm2\n\
\tandps\t%%xmm2,%%xmm1\n\
\tmov\t$%d,%%eax\n\
\tucomisd\t%%xmm1,%%xmm1\n\
\tjp\t9f\n\
\tmov\t$0x7fefffffffffffff,%%rdi\n\
\tmovq\t%%rdi,%%xmm2\n\
\tucomisd\t%%xmm2,%%xmm1\n\
\tja\t2f\n\
\tmov\t$0x0010000000000000,%%rdi\n\
\tmovq\t%%rdi,%%xmm2\n\
\tucomisd\t%%xmm2,%%xmm1\n\
\tjnb\t3f\n\
\txorps\t%%xmm1,%%xmm1\n\
\tucomisd\t%%xmm1,%%xmm0\n\
\tjp\t1f\n\
\tmovl\t$%d,%%eax\n\
\tje\t9f\n\
1:\tmovl\t$%d,%%eax\n\
\tjmp\t9f\n\
2:\tmovl\t$%d,%%eax\n\
\tjmp\t9f\n\
3:\tmovl\t$%d,%%eax\n\
9:"

#define FPCLASSIFY_LDOUBLE \
  "\tmov\t$%d,%%eax\n\
\tfld\t%%st\n\
\tfabs\n\
\tfucomi\t%%st,%%st\n\
\tjp\t6f\n\
\tpush\t$0x7ffe\n\
\tpush\t$-1\n\
\tfldt\t(%%rsp)\n\
\tadd\t$16,%%rsp\n\
\tfxch\n\
\tmov\t$%d,%%eax\n\
\tfucomi\n\
\tfstp\t%%st(1)\n\
\tja\t7f\n\
\tmov\t$1,%%edi\n\
\tpush\t%%rdi\n\
\tror\t%%rdi\n\
\tpush\t%%rdi\n\
\tfldt\t(%%rsp)\n\
\tadd\t$16,%%rsp\n\
\tfxch\n\
\tmov\t$%d,%%eax\n\
\tfucomip\n\
\tfstp\t%%st\n\
\tjnb\t8f\n\
\tfldz\n\
\tfxch\n\
\tfucomip\n\
\tfstp\t%%st\n\
\tjp\t5f\n\
\tmov\t$%d,%%eax\n\
\tje\t9f\n\
5:\tmov\t$%d,%%eax\n\
\tjmp\t9f\n\
6:\tfstp\t%%st\n\
\tfstp\t%%st\n\
\tjmp\t9f\n\
7:\tfstp\t%%st\n\
\tfstp\t%%st\n\
\tjmp\t9f\n\
8:\tfstp\t%%st\n\
9:"

void gen_fpclassify(FpClassify *fpc) {
  int fpnan = fpc->args[0];
  int fpinf = fpc->args[1];
  int fpnorm = fpc->args[2];
  int fpsubnorm = fpc->args[3];
  int fpzero = fpc->args[4];
  gen_expr(fpc->node);
  switch (fpc->node->ty->kind) {
    case TY_FLOAT:
      println(FPCLASSIFY_FLOAT, fpnan, fpzero, fpsubnorm, fpinf, fpnorm);
      break;
    case TY_DOUBLE:
      println(FPCLASSIFY_DOUBLE, fpnan, fpzero, fpsubnorm, fpinf, fpnorm);
      break;
    case TY_LDOUBLE:
      println(FPCLASSIFY_LDOUBLE, fpnan, fpinf, fpnorm, fpzero, fpsubnorm);
      break;
    default:
      UNREACHABLE();
  }
}
