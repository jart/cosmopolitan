#include "libc/macros.internal.h"
.text.unlikely

ekeyrevoked:
	.leafprologue
	.profilable
	mov	EKEYREVOKED(%rip),%ecx
	jmp	__errfun
	.endfn	ekeyrevoked,globl,hidden
