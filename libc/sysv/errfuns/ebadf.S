#include "libc/macros.h"
.text.unlikely

ebadf:	.leafprologue
	.profilable
	mov	EBADF(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadf,globl,hidden
