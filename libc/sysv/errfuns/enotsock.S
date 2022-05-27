#include "libc/macros.internal.h"
.text.unlikely

enotsock:
	.leafprologue
	.profilable
	mov	ENOTSOCK(%rip),%ecx
	jmp	__errfun
	.endfn	enotsock,globl,hidden
