#include "libc/macros.internal.h"
.text.unlikely

ecomm:	.leafprologue
	.profilable
	mov	ECOMM(%rip),%ecx
	jmp	__errfun
	.endfn	ecomm,globl,hidden
