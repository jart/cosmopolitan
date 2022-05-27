#include "libc/macros.internal.h"
.text.unlikely

econnrefused:
	.leafprologue
	.profilable
	mov	ECONNREFUSED(%rip),%ecx
	jmp	__errfun
	.endfn	econnrefused,globl,hidden
