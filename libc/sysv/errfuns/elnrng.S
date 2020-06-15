#include "libc/macros.h"
.text.unlikely

elnrng:	.leafprologue
	.profilable
	mov	ELNRNG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elnrng,globl,hidden
