#include "libc/macros.internal.h"
.text.unlikely

enosr:	.leafprologue
	.profilable
	mov	ENOSR(%rip),%ecx
	jmp	__errfun
	.endfn	enosr,globl,hidden
