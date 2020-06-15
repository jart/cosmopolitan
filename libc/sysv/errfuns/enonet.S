#include "libc/macros.h"
.text.unlikely

enonet:	.leafprologue
	.profilable
	mov	ENONET(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enonet,globl,hidden
