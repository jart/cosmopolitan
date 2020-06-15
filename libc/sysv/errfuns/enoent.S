#include "libc/macros.h"
.text.unlikely

enoent:	.leafprologue
	.profilable
	mov	ENOENT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoent,globl,hidden
