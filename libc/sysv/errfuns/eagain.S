#include "libc/macros.internal.h"
.text.unlikely

eagain:	.leafprologue
	.profilable
	mov	EAGAIN(%rip),%ecx
	jmp	__errfun
	.endfn	eagain,globl,hidden
