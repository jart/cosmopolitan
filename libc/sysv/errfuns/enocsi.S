#include "libc/macros.internal.h"
.text.unlikely

enocsi:	.leafprologue
	.profilable
	mov	ENOCSI(%rip),%ecx
	jmp	__errfun
	.endfn	enocsi,globl,hidden
