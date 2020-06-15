#include "libc/macros.h"
.text.unlikely

ebadslt:
	.leafprologue
	.profilable
	mov	EBADSLT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadslt,globl,hidden
