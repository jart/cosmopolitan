#include "libc/macros.internal.h"
.text.unlikely

enoexec:
	.leafprologue
	.profilable
	mov	ENOEXEC(%rip),%ecx
	jmp	__errfun
	.endfn	enoexec,globl,hidden
