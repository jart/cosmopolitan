#include "libc/macros.internal.h"
.text.unlikely

esrmnt:	.leafprologue
	.profilable
	mov	ESRMNT(%rip),%ecx
	jmp	__errfun
	.endfn	esrmnt,globl,hidden
