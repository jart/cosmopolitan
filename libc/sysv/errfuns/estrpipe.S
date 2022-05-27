#include "libc/macros.internal.h"
.text.unlikely

estrpipe:
	.leafprologue
	.profilable
	mov	ESTRPIPE(%rip),%ecx
	jmp	__errfun
	.endfn	estrpipe,globl,hidden
