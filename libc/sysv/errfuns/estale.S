#include "libc/macros.h"
.text.unlikely

estale:	.leafprologue
	.profilable
	mov	ESTALE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	estale,globl,hidden
