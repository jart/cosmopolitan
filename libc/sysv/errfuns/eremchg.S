#include "libc/macros.internal.h"
.text.unlikely

eremchg:
	.leafprologue
	.profilable
	mov	EREMCHG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremchg,globl,hidden
