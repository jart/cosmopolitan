#include "libc/macros.internal.h"
.text.unlikely

elibbad:
	.leafprologue
	.profilable
	mov	ELIBBAD(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibbad,globl,hidden
