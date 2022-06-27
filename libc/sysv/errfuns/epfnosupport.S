#include "libc/macros.internal.h"
.text.unlikely

epfnosupport:
	.leafprologue
	.profilable
	mov	EPFNOSUPPORT(%rip),%ecx
	jmp	__errfun
	.endfn	epfnosupport,globl,hidden
