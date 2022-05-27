#include "libc/macros.internal.h"
.text.unlikely

ebadfd:	.leafprologue
	.profilable
	mov	EBADFD(%rip),%ecx
	jmp	__errfun
	.endfn	ebadfd,globl,hidden
