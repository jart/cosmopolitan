#include "libc/macros.internal.h"
.text.unlikely

econnreset:
	.leafprologue
	.profilable
	mov	ECONNRESET(%rip),%ecx
	jmp	__errfun
	.endfn	econnreset,globl,hidden
