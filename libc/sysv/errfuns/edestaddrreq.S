#include "libc/macros.h"
.text.unlikely

edestaddrreq:
	.leafprologue
	.profilable
	mov	EDESTADDRREQ(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edestaddrreq,globl,hidden
