#include "libc/macros.h"
.text.unlikely

enodev:	.leafprologue
	.profilable
	mov	ENODEV(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enodev,globl,hidden
