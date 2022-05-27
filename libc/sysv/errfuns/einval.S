#include "libc/macros.internal.h"
.text.unlikely

einval:	.leafprologue
	.profilable
	mov	EINVAL(%rip),%ecx
	jmp	__errfun
	.endfn	einval,globl,hidden
