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
//	  - extern const char kWeekdayNameShort[7][4];
//	Type #2:
//	  - Double-NUL Terminated String
//	  - extern const char kWeekdayNameShort[];
	.section .rodata,"a",@progbits
	.underrun
kWeekdayNameShort:
	.asciz	"Sun"
	.asciz	"Mon"
	.asciz	"Tue"
	.asciz	"Wed"
	.asciz	"Thu"
	.asciz	"Fri"
	.asciz	"Sat"
	.byte	0
	.endobj	kWeekdayNameShort,globl
	.overrun
	.previous
