#include "libc/macros.internal.h"
.text.unlikely

enetreset:
	.leafprologue
	.profilable
	mov	ENETRESET(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enetreset,globl,hidden
