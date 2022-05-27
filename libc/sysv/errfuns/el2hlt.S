#include "libc/macros.internal.h"
.text.unlikely

el2hlt:	.leafprologue
	.profilable
	mov	EL2HLT(%rip),%ecx
	jmp	__errfun
	.endfn	el2hlt,globl,hidden
