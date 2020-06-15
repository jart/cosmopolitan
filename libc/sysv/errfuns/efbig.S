#include "libc/macros.h"
.text.unlikely

efbig:	.leafprologue
	.profilable
	mov	EFBIG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	efbig,globl,hidden
