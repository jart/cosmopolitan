#include "libc/macros.internal.h"
.text.unlikely

enetdown:
	.leafprologue
	.profilable
	mov	ENETDOWN(%rip),%ecx
	jmp	__errfun
	.endfn	enetdown,globl,hidden
