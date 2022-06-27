#include "libc/macros.internal.h"
.text.unlikely

elibbad:
	.leafprologue
	.profilable
	mov	ELIBBAD(%rip),%ecx
	jmp	__errfun
	.endfn	elibbad,globl,hidden
