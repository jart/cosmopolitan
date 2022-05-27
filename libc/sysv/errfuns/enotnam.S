#include "libc/macros.internal.h"
.text.unlikely

enotnam:
	.leafprologue
	.profilable
	mov	ENOTNAM(%rip),%ecx
	jmp	__errfun
	.endfn	enotnam,globl,hidden
