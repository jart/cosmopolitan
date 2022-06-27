#include "libc/macros.internal.h"
.text.unlikely

eoverflow:
	.leafprologue
	.profilable
	mov	EOVERFLOW(%rip),%ecx
	jmp	__errfun
	.endfn	eoverflow,globl,hidden
