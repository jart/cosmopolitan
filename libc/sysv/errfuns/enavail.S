#include "libc/macros.internal.h"
.text.unlikely

enavail:
	.leafprologue
	.profilable
	mov	ENAVAIL(%rip),%ecx
	jmp	__errfun
	.endfn	enavail,globl,hidden
