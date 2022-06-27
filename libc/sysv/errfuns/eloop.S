#include "libc/macros.internal.h"
.text.unlikely

eloop:	.leafprologue
	.profilable
	mov	ELOOP(%rip),%ecx
	jmp	__errfun
	.endfn	eloop,globl,hidden
