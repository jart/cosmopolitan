#include "libc/macros.internal.h"
.text.unlikely

enopkg:	.leafprologue
	.profilable
	mov	ENOPKG(%rip),%ecx
	jmp	__errfun
	.endfn	enopkg,globl,hidden
