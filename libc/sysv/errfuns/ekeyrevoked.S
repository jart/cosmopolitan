#include "libc/macros.h"
.text.unlikely

ekeyrevoked:
	.leafprologue
	.profilable
	mov	EKEYREVOKED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyrevoked,globl,hidden
