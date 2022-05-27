#include "libc/macros.internal.h"
.text.unlikely

ebadr:	.leafprologue
	.profilable
	mov	EBADR(%rip),%ecx
	jmp	__errfun
	.endfn	ebadr,globl,hidden
