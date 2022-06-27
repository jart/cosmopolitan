#include "libc/macros.internal.h"
.text.unlikely

etime:	.leafprologue
	.profilable
	mov	ETIME(%rip),%ecx
	jmp	__errfun
	.endfn	etime,globl,hidden
