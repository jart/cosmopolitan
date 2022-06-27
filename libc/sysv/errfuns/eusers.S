#include "libc/macros.internal.h"
.text.unlikely

eusers:	.leafprologue
	.profilable
	mov	EUSERS(%rip),%ecx
	jmp	__errfun
	.endfn	eusers,globl,hidden
