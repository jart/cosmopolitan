#include "libc/macros.internal.h"
.text.unlikely

enonet:	.leafprologue
	.profilable
	mov	ENONET(%rip),%ecx
	jmp	__errfun
	.endfn	enonet,globl,hidden
