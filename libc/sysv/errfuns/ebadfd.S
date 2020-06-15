#include "libc/macros.h"
.text.unlikely

ebadfd:	.leafprologue
	.profilable
	mov	EBADFD(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadfd,globl,hidden
