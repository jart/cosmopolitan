#include "libc/macros.h"
.text.unlikely

enotrecoverable:
	.leafprologue
	.profilable
	mov	ENOTRECOVERABLE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotrecoverable,globl,hidden
