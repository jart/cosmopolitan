#include "libc/macros.internal.h"
.text.unlikely

espipe:	.leafprologue
	.profilable
	mov	ESPIPE(%rip),%ecx
	jmp	__errfun
	.endfn	espipe,globl,hidden
