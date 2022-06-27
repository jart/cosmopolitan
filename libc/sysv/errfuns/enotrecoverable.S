#include "libc/macros.internal.h"
.text.unlikely

enotrecoverable:
	.leafprologue
	.profilable
	mov	ENOTRECOVERABLE(%rip),%ecx
	jmp	__errfun
	.endfn	enotrecoverable,globl,hidden
