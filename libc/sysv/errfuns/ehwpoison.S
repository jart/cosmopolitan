#include "libc/macros.internal.h"
.text.unlikely

ehwpoison:
	.leafprologue
	.profilable
	mov	EHWPOISON(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehwpoison,globl,hidden
