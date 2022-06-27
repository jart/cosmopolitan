#include "libc/macros.internal.h"
.text.unlikely

eacces:	.leafprologue
	.profilable
	mov	EACCES(%rip),%ecx
	jmp	__errfun
	.endfn	eacces,globl,hidden
