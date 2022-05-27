#include "libc/macros.internal.h"
.text.unlikely

enoano:	.leafprologue
	.profilable
	mov	ENOANO(%rip),%ecx
	jmp	__errfun
	.endfn	enoano,globl,hidden
