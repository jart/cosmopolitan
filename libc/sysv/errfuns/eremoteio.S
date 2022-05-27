#include "libc/macros.internal.h"
.text.unlikely

eremoteio:
	.leafprologue
	.profilable
	mov	EREMOTEIO(%rip),%ecx
	jmp	__errfun
	.endfn	eremoteio,globl,hidden
