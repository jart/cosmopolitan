#include "libc/macros.internal.h"
.text.unlikely

ecanceled:
	.leafprologue
	.profilable
	mov	ECANCELED(%rip),%ecx
	jmp	__errfun
	.endfn	ecanceled,globl,hidden
