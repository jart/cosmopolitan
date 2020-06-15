#include "libc/macros.h"
.text.unlikely

erestart:
	.leafprologue
	.profilable
	mov	ERESTART(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erestart,globl,hidden
