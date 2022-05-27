#include "libc/macros.internal.h"
.text.unlikely

eshutdown:
	.leafprologue
	.profilable
	mov	ESHUTDOWN(%rip),%ecx
	jmp	__errfun
	.endfn	eshutdown,globl,hidden
