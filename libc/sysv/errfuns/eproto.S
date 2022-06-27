#include "libc/macros.internal.h"
.text.unlikely

eproto:	.leafprologue
	.profilable
	mov	EPROTO(%rip),%ecx
	jmp	__errfun
	.endfn	eproto,globl,hidden
