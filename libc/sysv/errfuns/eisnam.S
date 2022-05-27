#include "libc/macros.internal.h"
.text.unlikely

eisnam:	.leafprologue
	.profilable
	mov	EISNAM(%rip),%ecx
	jmp	__errfun
	.endfn	eisnam,globl,hidden
