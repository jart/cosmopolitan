#include "libc/macros.internal.h"
.text.unlikely

enomem:	.leafprologue
	.profilable
	mov	ENOMEM(%rip),%ecx
	jmp	__errfun
	.endfn	enomem,globl,hidden
