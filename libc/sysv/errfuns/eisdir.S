#include "libc/macros.internal.h"
.text.unlikely

eisdir:	.leafprologue
	.profilable
	mov	EISDIR(%rip),%ecx
	jmp	__errfun
	.endfn	eisdir,globl,hidden
