#include "libc/macros.h"
.text.unlikely

enotuniq:
	.leafprologue
	.profilable
	mov	ENOTUNIQ(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotuniq,globl,hidden
