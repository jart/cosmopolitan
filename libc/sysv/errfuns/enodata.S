#include "libc/macros.internal.h"
.text.unlikely

enodata:
	.leafprologue
	.profilable
	mov	ENODATA(%rip),%ecx
	jmp	__errfun
	.endfn	enodata,globl,hidden
