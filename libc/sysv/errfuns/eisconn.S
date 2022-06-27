#include "libc/macros.internal.h"
.text.unlikely

eisconn:
	.leafprologue
	.profilable
	mov	EISCONN(%rip),%ecx
	jmp	__errfun
	.endfn	eisconn,globl,hidden
