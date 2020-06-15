#include "libc/macros.h"
.text.unlikely

enoano:	.leafprologue
	.profilable
	mov	ENOANO(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoano,globl,hidden
