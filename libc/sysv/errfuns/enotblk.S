#include "libc/macros.internal.h"
.text.unlikely

enotblk:
	.leafprologue
	.profilable
	mov	ENOTBLK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotblk,globl,hidden
