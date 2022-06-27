#include "libc/macros.internal.h"
.text.unlikely

elibscn:
	.leafprologue
	.profilable
	mov	ELIBSCN(%rip),%ecx
	jmp	__errfun
	.endfn	elibscn,globl,hidden
