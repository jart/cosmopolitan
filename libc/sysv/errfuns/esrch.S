#include "libc/macros.internal.h"
.text.unlikely

esrch:	.leafprologue
	.profilable
	mov	ESRCH(%rip),%ecx
	jmp	__errfun
	.endfn	esrch,globl,hidden
