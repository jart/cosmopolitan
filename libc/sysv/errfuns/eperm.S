#include "libc/macros.internal.h"
.text.unlikely

eperm:	.leafprologue
	.profilable
	mov	EPERM(%rip),%ecx
	jmp	__errfun
	.endfn	eperm,globl,hidden
