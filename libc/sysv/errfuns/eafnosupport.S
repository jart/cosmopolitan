#include "libc/macros.internal.h"
.text.unlikely

eafnosupport:
	.leafprologue
	.profilable
	mov	EAFNOSUPPORT(%rip),%ecx
	jmp	__errfun
	.endfn	eafnosupport,globl,hidden
