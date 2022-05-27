#include "libc/macros.internal.h"
.text.unlikely

elibmax:
	.leafprologue
	.profilable
	mov	ELIBMAX(%rip),%ecx
	jmp	__errfun
	.endfn	elibmax,globl,hidden
