#include "libc/macros.internal.h"
.text.unlikely

el2nsync:
	.leafprologue
	.profilable
	mov	EL2NSYNC(%rip),%ecx
	jmp	__errfun
	.endfn	el2nsync,globl,hidden
