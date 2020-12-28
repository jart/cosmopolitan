/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

/	Documentation for Intel(R)'s “Most Complicated Instruction”(TM)
/
/	mnemonic    op1  op2  op3  op4 modif f  description, notes
/	═══════════ ════ ════ ════ ═══ ════════ ═════════════════════════════
/	PCMPESTRM   XMM0 Vdq  Wdq  ... o..szapc Explicit Length, Return Mask
/	PCMPESTRI   rCX  Vdq  Wdq  ... o..szapc Explicit Length, Return Index
/	PCMPISTRM   XMM0 Vdq  Wdq  Ib  o..szapc Implicit Length, Return Mask
/	PCMPISTRI   rCX  Vdq  Wdq  Ib  o..szapc Implicit Length, Return Index
/
/	CF ← Reset if IntRes2 is equal to zero, set otherwise
/	ZF ← Set if any byte/word of xmm2/mem128 is null, reset otherwise
/	SF ← Set if any byte/word of xmm1 is null, reset otherwise
/	OF ← IntRes2[0]
/	AF ← Reset
/	PF ← Reset
/
/	PCMP{E,I}STR{I,M} Control Byte
/	@see Intel Manual V.2B §4.1.7
/
/	            ┌─0:index of the LEAST significant, set, bit is used
/	            │   regardless of corresponding input element validity
/	            │   intres2 is returned in least significant bits of xmm0
/	            ├─1:index of the MOST significant, set, bit is used
/	            │   regardless of corresponding input element validity
/	            │   each bit of intres2 is expanded to byte/word
/	            │┌─0:negation of intres1 is for all 16 (8) bits
/	            │├─1:negation of intres1 is masked by reg/mem validity
/	            ││┌─intres1 is negated (1’s complement)
/	            │││┌─mode{equalany,ranges,equaleach,equalordered}
/	            ││││ ┌─issigned
/	            ││││ │┌─is16bit
/	           u│││├┐││
.Lequalordered = 0b00001100
.Lequalorder16 = 0b00001101
.Lequalranges8 = 0b00000100
