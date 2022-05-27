#include "libc/macros.internal.h"
.text.unlikely

enotconn:
	.leafprologue
	.profilable
	mov	ENOTCONN(%rip),%ecx
	jmp	__errfun
	.endfn	enotconn,globl,hidden
