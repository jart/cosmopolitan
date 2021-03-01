#include "libc/macros.internal.h"
.text.unlikely

enolink:
	.leafprologue
	.profilable
	mov	ENOLINK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enolink,globl,hidden
