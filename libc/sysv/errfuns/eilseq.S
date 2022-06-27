#include "libc/macros.internal.h"
.text.unlikely

eilseq:	.leafprologue
	.profilable
	mov	EILSEQ(%rip),%ecx
	jmp	__errfun
	.endfn	eilseq,globl,hidden
