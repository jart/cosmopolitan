#include "libc/macros.h"
.text.unlikely

edom:	.leafprologue
	.profilable
	mov	EDOM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edom,globl,hidden
