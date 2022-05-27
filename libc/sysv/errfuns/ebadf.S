#include "libc/macros.internal.h"
.text.unlikely

ebadf:	.leafprologue
	.profilable
	mov	EBADF(%rip),%ecx
	jmp	__errfun
	.endfn	ebadf,globl,hidden
