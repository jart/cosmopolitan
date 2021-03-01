#include "libc/macros.internal.h"
.text.unlikely

e2big:	.leafprologue
	.profilable
	mov	E2BIG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	e2big,globl,hidden
