#include "libc/macros.internal.h"
.text.unlikely

elibacc:
	.leafprologue
	.profilable
	mov	ELIBACC(%rip),%ecx
	jmp	__errfun
	.endfn	elibacc,globl,hidden
