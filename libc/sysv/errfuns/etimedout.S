#include "libc/macros.h"
.text.unlikely

etimedout:
	.leafprologue
	.profilable
	mov	ETIMEDOUT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etimedout,globl,hidden
