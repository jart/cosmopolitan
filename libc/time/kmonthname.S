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

//	extern const char kMonthName[12][10];
	.section .rodata,"a",@progbits
	.underrun
kMonthName:
	.ascin	"January",10
	.ascin	"February",10
	.ascin	"March",10
	.ascin	"April",10
	.ascin	"May",10
	.ascin	"June",10
	.ascin	"July",10
	.ascin	"August",10
	.ascin	"September",10
	.ascin	"October",10
	.ascin	"November",10
	.ascin	"December",10
	.endobj	kMonthName,globl
	.overrun
	.previous
