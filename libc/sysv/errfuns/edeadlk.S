#include "libc/macros.h"
.text.unlikely

edeadlk:
	.leafprologue
	.profilable
	mov	EDEADLK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edeadlk,globl,hidden
