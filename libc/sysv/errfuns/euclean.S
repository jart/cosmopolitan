#include "libc/macros.internal.h"
.text.unlikely

euclean:
	.leafprologue
	.profilable
	mov	EUCLEAN(%rip),%ecx
	jmp	__errfun
	.endfn	euclean,globl,hidden
