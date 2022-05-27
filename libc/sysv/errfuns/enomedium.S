#include "libc/macros.internal.h"
.text.unlikely

enomedium:
	.leafprologue
	.profilable
	mov	ENOMEDIUM(%rip),%ecx
	jmp	__errfun
	.endfn	enomedium,globl,hidden
