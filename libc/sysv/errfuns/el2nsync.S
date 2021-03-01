#include "libc/macros.internal.h"
.text.unlikely

el2nsync:
	.leafprologue
	.profilable
	mov	EL2NSYNC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el2nsync,globl,hidden
