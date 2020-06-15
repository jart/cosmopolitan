#include "libc/macros.h"
.text.unlikely

eloop:	.leafprologue
	.profilable
	mov	ELOOP(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eloop,globl,hidden
