#include "libc/macros.internal.h"
.text.unlikely

eprotonosupport:
	.leafprologue
	.profilable
	mov	EPROTONOSUPPORT(%rip),%ecx
	jmp	__errfun
	.endfn	eprotonosupport,globl,hidden
