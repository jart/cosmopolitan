#include "libc/macros.h"
.text.unlikely

edotdot:
	.leafprologue
	.profilable
	mov	EDOTDOT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edotdot,globl,hidden
