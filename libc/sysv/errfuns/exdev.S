#include "libc/macros.internal.h"
.text.unlikely

exdev:	.leafprologue
	.profilable
	mov	EXDEV(%rip),%ecx
	jmp	__errfun
	.endfn	exdev,globl,hidden
