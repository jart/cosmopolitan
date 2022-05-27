#include "libc/macros.internal.h"
.text.unlikely

epipe:	.leafprologue
	.profilable
	mov	EPIPE(%rip),%ecx
	jmp	__errfun
	.endfn	epipe,globl,hidden
