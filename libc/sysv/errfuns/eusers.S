#include "libc/macros.h"
.text.unlikely

eusers:	.leafprologue
	.profilable
	mov	EUSERS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eusers,globl,hidden
