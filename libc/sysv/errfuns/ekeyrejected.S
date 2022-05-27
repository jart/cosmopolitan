#include "libc/macros.internal.h"
.text.unlikely

ekeyrejected:
	.leafprologue
	.profilable
	mov	EKEYREJECTED(%rip),%ecx
	jmp	__errfun
	.endfn	ekeyrejected,globl,hidden
