#include "libc/macros.h"
.text.unlikely

ecomm:	.leafprologue
	.profilable
	mov	ECOMM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ecomm,globl,hidden
