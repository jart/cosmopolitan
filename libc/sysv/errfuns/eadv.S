#include "libc/macros.internal.h"
.text.unlikely

eadv:	.leafprologue
	.profilable
	mov	EADV(%rip),%ecx
	jmp	__errfun
	.endfn	eadv,globl,hidden
