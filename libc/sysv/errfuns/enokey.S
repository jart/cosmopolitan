#include "libc/macros.internal.h"
.text.unlikely

enokey:	.leafprologue
	.profilable
	mov	ENOKEY(%rip),%ecx
	jmp	__errfun
	.endfn	enokey,globl,hidden
