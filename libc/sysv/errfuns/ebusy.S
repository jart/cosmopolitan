#include "libc/macros.h"
.text.unlikely

ebusy:	.leafprologue
	.profilable
	mov	EBUSY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebusy,globl,hidden
