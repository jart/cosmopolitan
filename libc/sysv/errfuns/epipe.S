#include "libc/macros.h"
.text.unlikely

epipe:	.leafprologue
	.profilable
	mov	EPIPE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	epipe,globl,hidden
