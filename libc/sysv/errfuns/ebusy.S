#include "libc/macros.internal.h"
.text.unlikely

ebusy:	.leafprologue
	.profilable
	mov	EBUSY(%rip),%ecx
	jmp	__errfun
	.endfn	ebusy,globl,hidden
