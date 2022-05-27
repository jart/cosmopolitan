#include "libc/macros.internal.h"
.text.unlikely

emlink:	.leafprologue
	.profilable
	mov	EMLINK(%rip),%ecx
	jmp	__errfun
	.endfn	emlink,globl,hidden
