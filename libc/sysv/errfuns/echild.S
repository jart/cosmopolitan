#include "libc/macros.internal.h"
.text.unlikely

echild:	.leafprologue
	.profilable
	mov	ECHILD(%rip),%ecx
	jmp	__errfun
	.endfn	echild,globl,hidden
