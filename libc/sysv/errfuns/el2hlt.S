#include "libc/macros.h"
.text.unlikely

el2hlt:	.leafprologue
	.profilable
	mov	EL2HLT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el2hlt,globl,hidden
