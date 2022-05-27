#include "libc/macros.internal.h"
.text.unlikely

enotdir:
	.leafprologue
	.profilable
	mov	ENOTDIR(%rip),%ecx
	jmp	__errfun
	.endfn	enotdir,globl,hidden
