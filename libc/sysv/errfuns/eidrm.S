#include "libc/macros.internal.h"
.text.unlikely

eidrm:	.leafprologue
	.profilable
	mov	EIDRM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eidrm,globl,hidden
