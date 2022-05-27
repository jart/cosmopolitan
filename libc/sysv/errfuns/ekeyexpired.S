#include "libc/macros.internal.h"
.text.unlikely

ekeyexpired:
	.leafprologue
	.profilable
	mov	EKEYEXPIRED(%rip),%ecx
	jmp	__errfun
	.endfn	ekeyexpired,globl,hidden
