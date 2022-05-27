#include "libc/macros.internal.h"
.text.unlikely

erestart:
	.leafprologue
	.profilable
	mov	ERESTART(%rip),%ecx
	jmp	__errfun
	.endfn	erestart,globl,hidden
