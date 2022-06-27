#include "libc/macros.internal.h"
.text.unlikely

econnaborted:
	.leafprologue
	.profilable
	mov	ECONNABORTED(%rip),%ecx
	jmp	__errfun
	.endfn	econnaborted,globl,hidden
