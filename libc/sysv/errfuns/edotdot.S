#include "libc/macros.internal.h"
.text.unlikely

edotdot:
	.leafprologue
	.profilable
	mov	EDOTDOT(%rip),%ecx
	jmp	__errfun
	.endfn	edotdot,globl,hidden
