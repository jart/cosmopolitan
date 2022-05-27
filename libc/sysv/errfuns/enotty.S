#include "libc/macros.internal.h"
.text.unlikely

enotty:	.leafprologue
	.profilable
	mov	ENOTTY(%rip),%ecx
	jmp	__errfun
	.endfn	enotty,globl,hidden
