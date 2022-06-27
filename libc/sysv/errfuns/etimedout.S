#include "libc/macros.internal.h"
.text.unlikely

etimedout:
	.leafprologue
	.profilable
	mov	ETIMEDOUT(%rip),%ecx
	jmp	__errfun
	.endfn	etimedout,globl,hidden
