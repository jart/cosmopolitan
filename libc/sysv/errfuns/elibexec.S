#include "libc/macros.h"
.text.unlikely

elibexec:
	.leafprologue
	.profilable
	mov	ELIBEXEC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibexec,globl,hidden
