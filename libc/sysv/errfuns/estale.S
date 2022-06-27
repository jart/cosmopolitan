#include "libc/macros.internal.h"
.text.unlikely

estale:	.leafprologue
	.profilable
	mov	ESTALE(%rip),%ecx
	jmp	__errfun
	.endfn	estale,globl,hidden
