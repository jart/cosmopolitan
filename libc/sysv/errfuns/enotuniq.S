#include "libc/macros.internal.h"
.text.unlikely

enotuniq:
	.leafprologue
	.profilable
	mov	ENOTUNIQ(%rip),%ecx
	jmp	__errfun
	.endfn	enotuniq,globl,hidden
