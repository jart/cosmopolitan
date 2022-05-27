#include "libc/macros.internal.h"
.text.unlikely

enostr:	.leafprologue
	.profilable
	mov	ENOSTR(%rip),%ecx
	jmp	__errfun
	.endfn	enostr,globl,hidden
