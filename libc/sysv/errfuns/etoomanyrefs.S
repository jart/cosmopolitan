#include "libc/macros.internal.h"
.text.unlikely

etoomanyrefs:
	.leafprologue
	.profilable
	mov	ETOOMANYREFS(%rip),%ecx
	jmp	__errfun
	.endfn	etoomanyrefs,globl,hidden
