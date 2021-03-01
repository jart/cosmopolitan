#include "libc/macros.internal.h"
.text.unlikely

elibacc:
	.leafprologue
	.profilable
	mov	ELIBACC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibacc,globl,hidden
