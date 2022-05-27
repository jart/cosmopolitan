#include "libc/macros.internal.h"
.text.unlikely

emediumtype:
	.leafprologue
	.profilable
	mov	EMEDIUMTYPE(%rip),%ecx
	jmp	__errfun
	.endfn	emediumtype,globl,hidden
