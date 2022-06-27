#include "libc/macros.internal.h"
.text.unlikely

eremote:
	.leafprologue
	.profilable
	mov	EREMOTE(%rip),%ecx
	jmp	__errfun
	.endfn	eremote,globl,hidden
