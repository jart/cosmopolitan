#include "libc/macros.h"
.text.unlikely

enokey:	.leafprologue
	.profilable
	mov	ENOKEY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enokey,globl,hidden
