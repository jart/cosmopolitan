#include "libc/macros.internal.h"
.text.unlikely

enametoolong:
	.leafprologue
	.profilable
	mov	ENAMETOOLONG(%rip),%ecx
	jmp	__errfun
	.endfn	enametoolong,globl,hidden
