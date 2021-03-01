#include "libc/macros.internal.h"
.text.unlikely

eownerdead:
	.leafprologue
	.profilable
	mov	EOWNERDEAD(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eownerdead,globl,hidden
