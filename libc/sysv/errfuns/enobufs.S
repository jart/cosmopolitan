#include "libc/macros.h"
.text.unlikely

enobufs:
	.leafprologue
	.profilable
	mov	ENOBUFS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enobufs,globl,hidden
