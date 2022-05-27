#include "libc/macros.internal.h"
.text.unlikely

edestaddrreq:
	.leafprologue
	.profilable
	mov	EDESTADDRREQ(%rip),%ecx
	jmp	__errfun
	.endfn	edestaddrreq,globl,hidden
