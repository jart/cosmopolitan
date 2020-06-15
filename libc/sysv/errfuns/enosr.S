#include "libc/macros.h"
.text.unlikely

enosr:	.leafprologue
	.profilable
	mov	ENOSR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enosr,globl,hidden
