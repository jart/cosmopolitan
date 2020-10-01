/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2018 Intel Corporation                                             │
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"

/	Phash tables for instruction length decoding.
/	@see	build/rle.py for more context here

	.initbss 300,_init_x86tab
xed_prefix_table_bit:
	.zero	32
	.endobj	xed_prefix_table_bit,globl,hidden
xed_has_modrm_2d:
	.zero	512
	.endobj	xed_has_modrm_2d,globl,hidden
xed_has_disp_regular:
	.zero	96
	.endobj	xed_has_disp_regular,globl,hidden
xed_has_sib_table:
	.zero	96
	.endobj	xed_has_sib_table,globl,hidden
xed_disp_bits_2d:
	.zero	512
	.endobj	xed_disp_bits_2d,globl,hidden
xed_imm_bits_2d:
	.zero	512
	.endobj	xed_imm_bits_2d,globl,hidden
	.previous

	.initro 300,_init_x86tab	# 536 bytes (30%)
xed_prefix_table_bit.rodata:
	.byte	4,0x00			# 00─03 ∅─♥
	.byte	4,0x40			# 04─07 ♦─•
	.byte	2,0xff			# 08─09 ◘─○
	.byte	2,0x00			# 0a─0b ◙─♂
	.byte	1,0xf0			# 0c─0c ♀
	.byte	17,0x00			# 0d─1d ♪─↔
	.byte	1,0x0d			# 1e─1e ▲
	.byte	1,0x00			# 1f─1f ▼
	.endobj	xed_prefix_table_bit.rodata

xed_has_modrm_2d.rodata:
	.byte	4,0x01			# 00─03 ∅─♥
	.byte	4,0x00			# 04─07 ♦─•
	.byte	4,0x01			# 08─0b ◘─♂
	.byte	3,0x00			# 0c─0e ♀─♫
	.byte	1,0x03			# 0f─0f ☼
	.byte	4,0x01			# 10─13 ►─‼
	.byte	4,0x00			# 14─17 ¶─↨
	.byte	4,0x01			# 18─1b ↑─←
	.byte	4,0x00			# 1c─1f ∟─▼
	.byte	4,0x01			# 20─23  ─#
	.byte	2,0x00			# 24─25 $─%
	.byte	1,0x03			# 26─26 &
	.byte	1,0x00			# 27─27 '
	.byte	4,0x01			# 28─2b (─+
	.byte	2,0x00			# 2c─2d ,──
	.byte	1,0x03			# 2e─2e .
	.byte	1,0x00			# 2f─2f /
	.byte	4,0x01			# 30─33 0─3
	.byte	2,0x00			# 34─35 4─5
	.byte	1,0x03			# 36─36 6
	.byte	1,0x00			# 37─37 7
	.byte	4,0x01			# 38─3b 8─;
	.byte	2,0x00			# 3c─3d <─=
	.byte	1,0x03			# 3e─3e >
	.byte	35,0x00			# 3f─61 ?─a
	.byte	2,0x01			# 62─63 b─c
	.byte	4,0x03			# 64─67 d─g
	.byte	1,0x00			# 68─68 h
	.byte	1,0x01			# 69─69 i
	.byte	1,0x00			# 6a─6a j
	.byte	1,0x01			# 6b─6b k
	.byte	20,0x00			# 6c─7f l─⌂
	.byte	16,0x01			# 80─8f Ç─Å
	.byte	48,0x00			# 90─bf É─┐
	.byte	2,0x01			# c0─c1 └─┴
	.byte	2,0x00			# c2─c3 ┬─├
	.byte	4,0x01			# c4─c7 ──╟
	.byte	8,0x00			# c8─cf ╚─╧
	.byte	4,0x01			# d0─d3 ╨─╙
	.byte	4,0x00			# d4─d7 ╘─╫
	.byte	8,0x01			# d8─df ╪─▀
	.byte	16,0x00			# e0─ef α─∩
	.byte	1,0x03			# f0─f0 ≡
	.byte	1,0x00			# f1─f1 ±
	.byte	2,0x03			# f2─f3 ≥─≤
	.byte	2,0x00			# f4─f5 ⌠─⌡
	.byte	2,0x01			# f6─f7 ÷─≈
	.byte	6,0x00			# f8─fd °─²
	.byte	6,0x01			# fe─103
	.byte	1,0x03			# 104─104
	.byte	5,0x00			# 105─109
	.byte	1,0x03			# 10a─10a
	.byte	1,0x00			# 10b─10b
	.byte	1,0x03			# 10c─10c
	.byte	1,0x01			# 10d─10d
	.byte	1,0x00			# 10e─10e
	.byte	1,0x03			# 10f─10f
	.byte	16,0x01			# 110─11f
	.byte	4,0x02			# 120─123
	.byte	4,0x03			# 124─127
	.byte	8,0x01			# 128─12f
	.byte	6,0x00			# 130─135
	.byte	1,0x03			# 136─136
	.byte	1,0x00			# 137─137
	.byte	8,0x03			# 138─13f
	.byte	55,0x01			# 140─176
	.byte	1,0x00			# 177─177
	.byte	8,0x01			# 178─17f
	.byte	16,0x00			# 180─18f
	.byte	16,0x01			# 190─19f
	.byte	3,0x00			# 1a0─1a2
	.byte	3,0x01			# 1a3─1a5
	.byte	2,0x03			# 1a6─1a7
	.byte	3,0x00			# 1a8─1aa
	.byte	29,0x01			# 1ab─1c7
	.byte	8,0x00			# 1c8─1cf
	.byte	48,0x01			# 1d0─1ff
	.endobj	xed_has_modrm_2d.rodata

xed_has_disp_regular.rodata:
	.byte	6,0x00			# 00─05 ∅─♣
	.byte	1,0x02			# 06─06 ♠
	.byte	1,0x00			# 07─07 •
	.byte	8,0x01			# 08─0f ◘─☼
	.byte	8,0x02			# 10─17 ►─↨
	.byte	13,0x00			# 18─24 ↑─$
	.byte	1,0x04			# 25─25 %
	.byte	2,0x00			# 26─27 &─'
	.byte	8,0x01			# 28─2f (─/
	.byte	8,0x04			# 30─37 0─7
	.byte	13,0x00			# 38─44 8─D
	.byte	1,0x04			# 45─45 E
	.byte	2,0x00			# 46─47 F─G
	.byte	8,0x01			# 48─4f H─O
	.byte	8,0x04			# 50─57 P─W
	.byte	8,0x00			# 58─5f X─_
	.endobj	xed_has_disp_regular.rodata

xed_has_sib_table.rodata:
	.byte	36,FALSE		# 00─23 ∅─#
	.byte	1,TRUE			# 24─24 $
	.byte	7,FALSE			# 25─2b %─+
	.byte	1,TRUE			# 2c─2c ,
	.byte	7,FALSE			# 2d─33 ──3
	.byte	1,TRUE			# 34─34 4
	.byte	15,FALSE		# 35─43 5─C
	.byte	1,TRUE			# 44─44 D
	.byte	7,FALSE			# 45─4b E─K
	.byte	1,TRUE			# 4c─4c L
	.byte	7,FALSE			# 4d─53 M─S
	.byte	1,TRUE			# 54─54 T
	.byte	11,FALSE		# 55─5f U─_
	.endobj	xed_has_sib_table.rodata

xed_disp_bits_2d.rodata:
	.byte	15,0x04			# 00─0e ∅─♫
	.byte	1,0x00			# 0f─0f ☼
	.byte	22,0x04			# 10─25 ►─%
	.byte	1,0x00			# 26─26 &
	.byte	7,0x04			# 27─2d '──
	.byte	1,0x00			# 2e─2e .
	.byte	7,0x04			# 2f─35 /─5
	.byte	1,0x00			# 36─36 6
	.byte	7,0x04			# 37─3d 7─=
	.byte	1,0x00			# 3e─3e >
	.byte	37,0x04			# 3f─63 ?─c
	.byte	4,0x00			# 64─67 d─g
	.byte	8,0x04			# 68─6f h─o
	.byte	16,0x01			# 70─7f p─⌂
	.byte	26,0x04			# 80─99 Ç─Ö
	.byte	1,0x02			# 9a─9a Ü
	.byte	5,0x04			# 9b─9f ¢─ƒ
	.byte	4,0x05			# a0─a3 á─ú
	.byte	35,0x04			# a4─c6 ñ─╞
	.byte	1,0x06			# c7─c7 ╟
	.byte	24,0x04			# c8─df ╚─▀
	.byte	4,0x01			# e0─e3 α─π
	.byte	4,0x04			# e4─e7 Σ─τ
	.byte	2,0x03			# e8─e9 Φ─Θ
	.byte	1,0x02			# ea─ea Ω
	.byte	1,0x01			# eb─eb δ
	.byte	4,0x04			# ec─ef ∞─∩
	.byte	1,0x00			# f0─f0 ≡
	.byte	1,0x04			# f1─f1 ±
	.byte	2,0x00			# f2─f3 ≥─≤
	.byte	16,0x04			# f4─103
	.byte	1,0x00			# 104─104
	.byte	5,0x04			# 105─109
	.byte	1,0x00			# 10a─10a
	.byte	1,0x04			# 10b─10b
	.byte	1,0x00			# 10c─10c
	.byte	2,0x04			# 10d─10e
	.byte	1,0x00			# 10f─10f
	.byte	20,0x04			# 110─123
	.byte	4,0x00			# 124─127
	.byte	14,0x04			# 128─135
	.byte	1,0x00			# 136─136
	.byte	1,0x04			# 137─137
	.byte	8,0x00			# 138─13f
	.byte	64,0x04			# 140─17f
	.byte	16,0x03			# 180─18f
	.byte	22,0x04			# 190─1a5
	.byte	2,0x00			# 1a6─1a7
	.byte	88,0x04			# 1a8─1ff
	.endobj	xed_disp_bits_2d.rodata

xed_imm_bits_2d.rodata:
	.byte	4,0x01			# 00─03 ∅─♥
	.byte	1,0x05			# 04─04 ♦
	.byte	1,0x07			# 05─05 ♣
	.byte	6,0x01			# 06─0b ♠─♂
	.byte	1,0x09			# 0c─0c ♀
	.byte	1,0x07			# 0d─0d ♪
	.byte	1,0x01			# 0e─0e ♫
	.byte	1,0x00			# 0f─0f ☼
	.byte	4,0x01			# 10─13 ►─‼
	.byte	1,0x05			# 14─14 ¶
	.byte	1,0x07			# 15─15 §
	.byte	6,0x01			# 16─1b ▬─←
	.byte	1,0x05			# 1c─1c ∟
	.byte	1,0x07			# 1d─1d ↔
	.byte	6,0x01			# 1e─23 ▲─#
	.byte	1,0x05			# 24─24 $
	.byte	1,0x07			# 25─25 %
	.byte	1,0x00			# 26─26 &
	.byte	5,0x01			# 27─2b '─+
	.byte	1,0x05			# 2c─2c ,
	.byte	1,0x07			# 2d─2d ─
	.byte	1,0x00			# 2e─2e .
	.byte	5,0x01			# 2f─33 /─3
	.byte	1,0x09			# 34─34 4
	.byte	1,0x07			# 35─35 5
	.byte	1,0x00			# 36─36 6
	.byte	5,0x01			# 37─3b 7─;
	.byte	1,0x05			# 3c─3c <
	.byte	1,0x07			# 3d─3d =
	.byte	1,0x00			# 3e─3e >
	.byte	37,0x01			# 3f─63 ?─c
	.byte	4,0x00			# 64─67 d─g
	.byte	1,0x06			# 68─68 h
	.byte	1,0x07			# 69─69 i
	.byte	2,0x05			# 6a─6b j─k
	.byte	20,0x01			# 6c─7f l─⌂	
	.byte	1,0x05			# 80─80 Ç
	.byte	1,0x07			# 81─81 ü
	.byte	2,0x05			# 82─83 é─â
	.byte	22,0x01			# 84─99 ä─Ö
	.byte	1,0x08			# 9a─9a Ü
	.byte	13,0x01			# 9b─a7 ¢─º
	.byte	1,0x05			# a8─a8 ¿
	.byte	1,0x07			# a9─a9 ⌐
	.byte	6,0x01			# aa─af ¬─»
	.byte	8,0x09			# b0─b7 ░─╖
	.byte	8,0x0a			# b8─bf ╕─┐
	.byte	2,0x09			# c0─c1 └─┴
	.byte	1,0x08			# c2─c2 ┬
	.byte	3,0x01			# c3─c5 ├─┼
	.byte	1,0x09			# c6─c6 ╞
	.byte	1,0x02			# c7─c7 ╟
	.byte	1,0x0b			# c8─c8 ╚
	.byte	1,0x01			# c9─c9 ╔
	.byte	1,0x08			# ca─ca ╩
	.byte	2,0x01			# cb─cc ╦─╠
	.byte	1,0x09			# cd─cd ═
	.byte	6,0x01			# ce─d3 ╬─╙
	.byte	2,0x09			# d4─d5 ╘─╒
	.byte	14,0x01			# d6─e3 ╓─π
	.byte	4,0x09			# e4─e7 Σ─τ
	.byte	2,0x01			# e8─e9 Φ─Θ
	.byte	1,0x08			# ea─ea Ω
	.byte	5,0x01			# eb─ef δ─∩
	.byte	1,0x00			# f0─f0 ≡
	.byte	1,0x01			# f1─f1 ±
	.byte	2,0x00			# f2─f3 ≥─≤
	.byte	2,0x01			# f4─f5 ⌠─⌡
	.byte	1,0x03			# f6─f6 ÷
	.byte	1,0x04			# f7─f7 ≈
	.byte	12,0x01			# f8─103
	.byte	1,0x00			# 104─104
	.byte	5,0x01			# 105─109
	.byte	1,0x00			# 10a─10a
	.byte	1,0x01			# 10b─10b
	.byte	1,0x00			# 10c─10c
	.byte	2,0x01			# 10d─10e
	.byte	1,0x00			# 10f─10f
	.byte	20,0x01			# 110─123
	.byte	4,0x00			# 124─127
	.byte	14,0x01			# 128─135
	.byte	1,0x00			# 136─136
	.byte	1,0x01			# 137─137
	.byte	8,0x00			# 138─13f
	.byte	48,0x01			# 140─16f
	.byte	4,0x09			# 170─173
	.byte	4,0x01			# 174─177
	.byte	1,0x0c			# 178─178
	.byte	43,0x01			# 179─1a3
	.byte	1,0x09			# 1a4─1a4
	.byte	1,0x01			# 1a5─1a5
	.byte	2,0x00			# 1a6─1a7
	.byte	4,0x01			# 1a8─1ab
	.byte	1,0x09			# 1ac─1ac
	.byte	13,0x01			# 1ad─1b9
	.byte	1,0x09			# 1ba─1ba
	.byte	7,0x01			# 1bb─1c1
	.byte	1,0x09			# 1c2─1c2
	.byte	1,0x01			# 1c3─1c3
	.byte	3,0x09			# 1c4─1c6
	.byte	57,0x01			# 1c7─1ff
	.endobj	xed_imm_bits_2d.rodata

	.byte	0,0			# terminator
	.byte	0,0,0,0,0,0		# padding
	.previous

	.init.start 300,_init_x86tab
	call	rldecode
	lodsl
	lodsw
	.init.end 300,_init_x86tab
