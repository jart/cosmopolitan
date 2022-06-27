#include "libc/macros.internal.h"
.text.unlikely

enetunreach:
	.leafprologue
	.profilable
	mov	ENETUNREACH(%rip),%ecx
	jmp	__errfun
	.endfn	enetunreach,globl,hidden
