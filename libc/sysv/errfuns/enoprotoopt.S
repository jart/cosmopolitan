#include "libc/macros.internal.h"
.text.unlikely

enoprotoopt:
	.leafprologue
	.profilable
	mov	ENOPROTOOPT(%rip),%ecx
	jmp	__errfun
	.endfn	enoprotoopt,globl,hidden
