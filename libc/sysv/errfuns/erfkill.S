#include "libc/macros.internal.h"
.text.unlikely

erfkill:
	.leafprologue
	.profilable
	mov	ERFKILL(%rip),%ecx
	jmp	__errfun
	.endfn	erfkill,globl,hidden
