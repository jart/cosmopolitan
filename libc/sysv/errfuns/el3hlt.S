#include "libc/macros.internal.h"
.text.unlikely

el3hlt:	.leafprologue
	.profilable
	mov	EL3HLT(%rip),%ecx
	jmp	__errfun
	.endfn	el3hlt,globl,hidden
