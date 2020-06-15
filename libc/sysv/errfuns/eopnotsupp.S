#include "libc/macros.h"
.text.unlikely

eopnotsupp:
	.leafprologue
	.profilable
	mov	EOPNOTSUPP(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eopnotsupp,globl,hidden
