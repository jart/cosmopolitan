#include "libc/macros.h"
.text.unlikely

eexist:	.leafprologue
	.profilable
	mov	EEXIST(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eexist,globl,hidden
