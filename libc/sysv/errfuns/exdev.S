#include "libc/macros.h"
.text.unlikely

exdev:	.leafprologue
	.profilable
	mov	EXDEV(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	exdev,globl,hidden
