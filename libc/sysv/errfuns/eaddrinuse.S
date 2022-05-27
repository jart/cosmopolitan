#include "libc/macros.internal.h"
.text.unlikely

eaddrinuse:
	.leafprologue
	.profilable
	mov	EADDRINUSE(%rip),%ecx
	jmp	__errfun
	.endfn	eaddrinuse,globl,hidden
