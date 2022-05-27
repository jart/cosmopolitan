#include "libc/macros.internal.h"
.text.unlikely

enobufs:
	.leafprologue
	.profilable
	mov	ENOBUFS(%rip),%ecx
	jmp	__errfun
	.endfn	enobufs,globl,hidden
