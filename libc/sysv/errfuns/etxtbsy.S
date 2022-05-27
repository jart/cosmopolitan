#include "libc/macros.internal.h"
.text.unlikely

etxtbsy:
	.leafprologue
	.profilable
	mov	ETXTBSY(%rip),%ecx
	jmp	__errfun
	.endfn	etxtbsy,globl,hidden
