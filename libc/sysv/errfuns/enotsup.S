#include "libc/macros.h"
.text.unlikely

enotsup:
	.leafprologue
	.profilable
	mov	ENOTSUP(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotsup,globl,hidden
