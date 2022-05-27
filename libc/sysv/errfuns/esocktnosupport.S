#include "libc/macros.internal.h"
.text.unlikely

esocktnosupport:
	.leafprologue
	.profilable
	mov	ESOCKTNOSUPPORT(%rip),%ecx
	jmp	__errfun
	.endfn	esocktnosupport,globl,hidden
