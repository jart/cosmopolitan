#include "libc/macros.internal.h"
.text.unlikely

ebade:	.leafprologue
	.profilable
	mov	EBADE(%rip),%ecx
	jmp	__errfun
	.endfn	ebade,globl,hidden
