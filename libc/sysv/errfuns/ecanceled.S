#include "libc/macros.internal.h"
.text.unlikely

ecanceled:
	.leafprologue
	.profilable
	mov	ECANCELED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ecanceled,globl,hidden
