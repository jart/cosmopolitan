#include "libc/macros.internal.h"
.text.unlikely

enolink:
	.leafprologue
	.profilable
	mov	ENOLINK(%rip),%ecx
	jmp	__errfun
	.endfn	enolink,globl,hidden
