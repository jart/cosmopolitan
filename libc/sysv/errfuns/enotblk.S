#include "libc/macros.internal.h"
.text.unlikely

enotblk:
	.leafprologue
	.profilable
	mov	ENOTBLK(%rip),%ecx
	jmp	__errfun
	.endfn	enotblk,globl,hidden
