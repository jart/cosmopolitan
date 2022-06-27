#include "libc/macros.internal.h"
.text.unlikely

ehostunreach:
	.leafprologue
	.profilable
	mov	EHOSTUNREACH(%rip),%ecx
	jmp	__errfun
	.endfn	ehostunreach,globl,hidden
