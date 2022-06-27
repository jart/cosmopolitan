#include "libc/macros.internal.h"
.text.unlikely

erange:	.leafprologue
	.profilable
	mov	ERANGE(%rip),%ecx
	jmp	__errfun
	.endfn	erange,globl,hidden
