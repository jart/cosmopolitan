#include "libc/macros.internal.h"
.text.unlikely

einprogress:
	.leafprologue
	.profilable
	mov	EINPROGRESS(%rip),%ecx
	jmp	__errfun
	.endfn	einprogress,globl,hidden
