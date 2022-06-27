#include "libc/macros.internal.h"
.text.unlikely

enoent:	.leafprologue
	.profilable
	mov	ENOENT(%rip),%ecx
	jmp	__errfun
	.endfn	enoent,globl,hidden
