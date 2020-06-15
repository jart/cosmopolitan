#include "libc/macros.h"
.text.unlikely

eperm:	.leafprologue
	.profilable
	mov	EPERM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eperm,globl,hidden
