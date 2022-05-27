#include "libc/macros.internal.h"
.text.unlikely

edom:	.leafprologue
	.profilable
	mov	EDOM(%rip),%ecx
	jmp	__errfun
	.endfn	edom,globl,hidden
