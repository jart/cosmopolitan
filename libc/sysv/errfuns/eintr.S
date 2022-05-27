#include "libc/macros.internal.h"
.text.unlikely

eintr:	.leafprologue
	.profilable
	mov	EINTR(%rip),%ecx
	jmp	__errfun
	.endfn	eintr,globl,hidden
