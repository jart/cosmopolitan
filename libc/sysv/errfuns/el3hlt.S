#include "libc/macros.h"
.text.unlikely

el3hlt:	.leafprologue
	.profilable
	mov	EL3HLT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el3hlt,globl,hidden
