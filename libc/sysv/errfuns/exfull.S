#include "libc/macros.internal.h"
.text.unlikely

exfull:	.leafprologue
	.profilable
	mov	EXFULL(%rip),%ecx
	jmp	__errfun
	.endfn	exfull,globl,hidden
