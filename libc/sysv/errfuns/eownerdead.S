#include "libc/macros.internal.h"
.text.unlikely

eownerdead:
	.leafprologue
	.profilable
	mov	EOWNERDEAD(%rip),%ecx
	jmp	__errfun
	.endfn	eownerdead,globl,hidden
