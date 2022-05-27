#include "libc/macros.internal.h"
.text.unlikely

ebadslt:
	.leafprologue
	.profilable
	mov	EBADSLT(%rip),%ecx
	jmp	__errfun
	.endfn	ebadslt,globl,hidden
