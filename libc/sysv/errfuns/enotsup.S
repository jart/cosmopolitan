#include "libc/macros.internal.h"
.text.unlikely

enotsup:
	.leafprologue
	.profilable
	mov	ENOTSUP(%rip),%ecx
	jmp	__errfun
	.endfn	enotsup,globl,hidden
