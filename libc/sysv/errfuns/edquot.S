#include "libc/macros.internal.h"
.text.unlikely

edquot:	.leafprologue
	.profilable
	mov	EDQUOT(%rip),%ecx
	jmp	__errfun
	.endfn	edquot,globl,hidden
