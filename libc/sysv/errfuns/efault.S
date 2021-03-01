#include "libc/macros.internal.h"
.text.unlikely

efault:	.leafprologue
	.profilable
	mov	EFAULT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	efault,globl,hidden
