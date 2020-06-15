#include "libc/macros.h"
.text.unlikely

echrng:	.leafprologue
	.profilable
	mov	ECHRNG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	echrng,globl,hidden
