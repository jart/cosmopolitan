#include "libc/macros.internal.h"
.text.unlikely

ehostdown:
	.leafprologue
	.profilable
	mov	EHOSTDOWN(%rip),%ecx
	jmp	__errfun
	.endfn	ehostdown,globl,hidden
