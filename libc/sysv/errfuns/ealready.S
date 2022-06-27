#include "libc/macros.internal.h"
.text.unlikely

ealready:
	.leafprologue
	.profilable
	mov	EALREADY(%rip),%ecx
	jmp	__errfun
	.endfn	ealready,globl,hidden
