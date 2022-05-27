#include "libc/macros.internal.h"
.text.unlikely

enospc:	.leafprologue
	.profilable
	mov	ENOSPC(%rip),%ecx
	jmp	__errfun
	.endfn	enospc,globl,hidden
