/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
