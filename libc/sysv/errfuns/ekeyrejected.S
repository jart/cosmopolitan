#include "libc/macros.h"
.text.unlikely

ekeyrejected:
	.leafprologue
	.profilable
	mov	EKEYREJECTED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyrejected,globl,hidden
