#include "libc/macros.h"
.text.unlikely

elibmax:
	.leafprologue
	.profilable
	mov	ELIBMAX(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibmax,globl,hidden
