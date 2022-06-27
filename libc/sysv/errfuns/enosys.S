#include "libc/macros.internal.h"
.text.unlikely

enosys:	.leafprologue
	.profilable
	mov	ENOSYS(%rip),%ecx
	jmp	__errfun
	.endfn	enosys,globl,hidden
