#include "libc/macros.internal.h"
.text.unlikely

eexist:	.leafprologue
	.profilable
	mov	EEXIST(%rip),%ecx
	jmp	__errfun
	.endfn	eexist,globl,hidden
