#include "libc/macros.h"
.text.unlikely

edquot:	.leafprologue
	.profilable
	mov	EDQUOT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edquot,globl,hidden
