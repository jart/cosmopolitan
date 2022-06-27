#include "libc/macros.internal.h"
.text.unlikely

eunatch:
	.leafprologue
	.profilable
	mov	EUNATCH(%rip),%ecx
	jmp	__errfun
	.endfn	eunatch,globl,hidden
