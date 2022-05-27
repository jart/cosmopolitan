#include "libc/macros.internal.h"
.text.unlikely

enotempty:
	.leafprologue
	.profilable
	mov	ENOTEMPTY(%rip),%ecx
	jmp	__errfun
	.endfn	enotempty,globl,hidden
