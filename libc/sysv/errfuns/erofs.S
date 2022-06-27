#include "libc/macros.internal.h"
.text.unlikely

erofs:	.leafprologue
	.profilable
	mov	EROFS(%rip),%ecx
	jmp	__errfun
	.endfn	erofs,globl,hidden
