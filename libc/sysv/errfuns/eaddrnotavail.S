#include "libc/macros.internal.h"
.text.unlikely

eaddrnotavail:
	.leafprologue
	.profilable
	mov	EADDRNOTAVAIL(%rip),%ecx
	jmp	__errfun
	.endfn	eaddrnotavail,globl,hidden
