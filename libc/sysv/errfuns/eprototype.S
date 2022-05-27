#include "libc/macros.internal.h"
.text.unlikely

eprototype:
	.leafprologue
	.profilable
	mov	EPROTOTYPE(%rip),%ecx
	jmp	__errfun
	.endfn	eprototype,globl,hidden
