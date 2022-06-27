#include "libc/macros.internal.h"
.text.unlikely

emfile:	.leafprologue
	.profilable
	mov	EMFILE(%rip),%ecx
	jmp	__errfun
	.endfn	emfile,globl,hidden
