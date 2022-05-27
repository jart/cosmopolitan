#include "libc/macros.internal.h"
.text.unlikely

elibexec:
	.leafprologue
	.profilable
	mov	ELIBEXEC(%rip),%ecx
	jmp	__errfun
	.endfn	elibexec,globl,hidden
