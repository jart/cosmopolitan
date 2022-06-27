#include "libc/macros.internal.h"
.text.unlikely

echrng:	.leafprologue
	.profilable
	mov	ECHRNG(%rip),%ecx
	jmp	__errfun
	.endfn	echrng,globl,hidden
