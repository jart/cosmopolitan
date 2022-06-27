#include "libc/macros.internal.h"
.text.unlikely

efbig:	.leafprologue
	.profilable
	mov	EFBIG(%rip),%ecx
	jmp	__errfun
	.endfn	efbig,globl,hidden
