#include "libc/macros.internal.h"
.text.unlikely

enodev:	.leafprologue
	.profilable
	mov	ENODEV(%rip),%ecx
	jmp	__errfun
	.endfn	enodev,globl,hidden
