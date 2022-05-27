#include "libc/macros.internal.h"
.text.unlikely

edeadlk:
	.leafprologue
	.profilable
	mov	EDEADLK(%rip),%ecx
	jmp	__errfun
	.endfn	edeadlk,globl,hidden
