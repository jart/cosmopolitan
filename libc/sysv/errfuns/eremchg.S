#include "libc/macros.internal.h"
.text.unlikely

eremchg:
	.leafprologue
	.profilable
	mov	EREMCHG(%rip),%ecx
	jmp	__errfun
	.endfn	eremchg,globl,hidden
