#include "libc/macros.internal.h"
.text.unlikely

ehwpoison:
	.leafprologue
	.profilable
	mov	EHWPOISON(%rip),%ecx
	jmp	__errfun
	.endfn	ehwpoison,globl,hidden
