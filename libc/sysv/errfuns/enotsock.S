#include "libc/macros.h"
.text.unlikely

enotsock:
	.leafprologue
	.profilable
	mov	ENOTSOCK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotsock,globl,hidden
