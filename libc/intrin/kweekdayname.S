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

//	extern const char kWeekdayName[7][10];
	.section .rodata,"a",@progbits
	.underrun
kWeekdayName:
	.ascin	"Sunday",10
	.ascin	"Monday",10
	.ascin	"Tuesday",10
	.ascin	"Wednesday",10
	.ascin	"Thursday",10
	.ascin	"Friday",10
	.ascin	"Saturday",10
	.endobj	kWeekdayName,globl
	.overrun
	.previous
