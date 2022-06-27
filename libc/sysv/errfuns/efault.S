#include "libc/macros.internal.h"
.text.unlikely

efault:	.leafprologue
	.profilable
	mov	EFAULT(%rip),%ecx
	jmp	__errfun
	.endfn	efault,globl,hidden
