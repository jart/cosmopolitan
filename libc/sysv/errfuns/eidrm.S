#include "libc/macros.internal.h"
.text.unlikely

eidrm:	.leafprologue
	.profilable
	mov	EIDRM(%rip),%ecx
	jmp	__errfun
	.endfn	eidrm,globl,hidden
