#include "libc/macros.internal.h"
.text.unlikely

e2big:	.leafprologue
	.profilable
	mov	E2BIG(%rip),%ecx
	jmp	__errfun
	.endfn	e2big,globl,hidden
