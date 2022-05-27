#include "libc/macros.internal.h"
.text.unlikely

enxio:	.leafprologue
	.profilable
	mov	ENXIO(%rip),%ecx
	jmp	__errfun
	.endfn	enxio,globl,hidden
