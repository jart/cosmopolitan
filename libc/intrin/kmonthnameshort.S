#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/macros.internal.h"

//	Type #1:
//	  - Indexable C-String Array
//	  - extern const char kMonthNameShort[12][4];
//	Type #2:
//	  - Double-NUL Terminated String
//	  - extern const char kMonthNameShort[];
	.section .rodata,"a",@progbits
	.underrun
kMonthNameShort:
	.ascin	"Jan",4
	.ascin	"Feb",4
	.ascin	"Mar",4
	.ascin	"Apr",4
	.ascin	"May",4
	.ascin	"Jun",4
	.ascin	"Jul",4
	.ascin	"Aug",4
	.ascin	"Sep",4
	.ascin	"Oct",4
	.ascin	"Nov",4
	.ascin	"Dec",4
	.byte	0
	.endobj	kMonthNameShort,globl
	.overrun
	.previous
