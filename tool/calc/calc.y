/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
%include {
#include "libc/stdio/stdio.h"
#include "tool/calc/calc.h"
#include "libc/calls/calls.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "libc/runtime/gc.h"
#include "libc/math.h"
}

%token_type {struct Token}
%type number {long double}
%syntax_error { SyntaxError(); }

%left LOR.
%left LAND.
%left OR.
%left XOR.
%left AND.
%left EQ NE.
%left LT LE GT GE.
%left SHL SHR.
%left PLUS MINUS.
%left MUL DIV DDIV REM.
%right NOT LNOT.
%right EXP.

program ::= number.
number(A) ::= NUMBER(B). { A = ParseNumber(B); }
number(A) ::= LP number(B) RP. { A = B; }
number(A) ::= LNOT number(B). { A = !B; }
number(A) ::= NOT number(B). { A = ~(long)B; }
number(A) ::= PLUS number(B). { A = +B; } [NOT]
number(A) ::= MINUS number(B). { A = -B; } [NOT]
number(A) ::= SYMBOL(F) LP numbers(N) RP. { A = CallFunction(F, N); }
number(A) ::= number(B) EQ number(C). { A = B == C; }
number(A) ::= number(B) NE number(C). { A = B != C; }
number(A) ::= number(B) LT number(C). { A = B < C; }
number(A) ::= number(B) LE number(C). { A = B <= C; }
number(A) ::= number(B) GT number(C). { A = B > C; }
number(A) ::= number(B) GE number(C). { A = B >= C; }
number(A) ::= number(B) LOR number(C). { A = B || C; }
number(A) ::= number(B) LAND number(C). { A = B && C; }
number(A) ::= number(B) PLUS number(C). { A = B + C; }
number(A) ::= number(B) MINUS number(C). { A = B - C; }
number(A) ::= number(B) MUL number(C). { A = B * C; }
number(A) ::= number(B) DIV number(C). { A = B / C; }
number(A) ::= number(B) REM number(C). { A = remainderl(B, C); }
number(A) ::= number(B) EXP number(C). { A = powl(B, C); }
number(A) ::= number(B) DDIV number(C). { A = truncl(B / C); }
number(A) ::= number(B) OR number(C). { A = (long)B | (long)C; }
number(A) ::= number(B) XOR number(C). { A = (long)B ^ (long)C; }
number(A) ::= number(B) AND number(C). { A = (long)B & (long)C; }
number(A) ::= number(B) SHL number(C). { A = (long)B << (long)C; }
number(A) ::= number(B) SHR number(C). { A = (long)B >> (long)C; }

%type numbers {struct Numbers *}
%destructor numbers { NumbersFree($$); }
numbers(A) ::= . { A = 0; }
numbers(A) ::= number(B). { A = NumbersAppend(0, B); }
numbers(A) ::= numbers(A) COMMA number(B). { A = NumbersAppend(A, B); }
