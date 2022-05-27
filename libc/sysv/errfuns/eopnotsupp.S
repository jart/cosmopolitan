#include "libc/macros.internal.h"
.text.unlikely

eopnotsupp:
	.leafprologue
	.profilable
	mov	EOPNOTSUPP(%rip),%ecx
	jmp	__errfun
	.endfn	eopnotsupp,globl,hidden
