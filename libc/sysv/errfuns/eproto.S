#include "libc/macros.h"
.text.unlikely

eproto:	.leafprologue
	.profilable
	mov	EPROTO(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eproto,globl,hidden
