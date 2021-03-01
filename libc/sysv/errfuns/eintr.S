#include "libc/macros.internal.h"
.text.unlikely

eintr:	.leafprologue
	.profilable
	mov	EINTR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eintr,globl,hidden
