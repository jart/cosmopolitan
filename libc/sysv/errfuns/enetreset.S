#include "libc/macros.internal.h"
.text.unlikely

enetreset:
	.leafprologue
	.profilable
	mov	ENETRESET(%rip),%ecx
	jmp	__errfun
	.endfn	enetreset,globl,hidden
