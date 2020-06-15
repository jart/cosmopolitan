#include "libc/macros.h"
.text.unlikely

enomedium:
	.leafprologue
	.profilable
	mov	ENOMEDIUM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomedium,globl,hidden
