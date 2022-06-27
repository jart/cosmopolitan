#include "libc/macros.internal.h"
.text.unlikely

enfile:	.leafprologue
	.profilable
	mov	ENFILE(%rip),%ecx
	jmp	__errfun
	.endfn	enfile,globl,hidden
