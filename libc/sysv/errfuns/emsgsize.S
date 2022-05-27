#include "libc/macros.internal.h"
.text.unlikely

emsgsize:
	.leafprologue
	.profilable
	mov	EMSGSIZE(%rip),%ecx
	jmp	__errfun
	.endfn	emsgsize,globl,hidden
