#include "libc/macros.h"
.text.unlikely

enodata:
	.leafprologue
	.profilable
	mov	ENODATA(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enodata,globl,hidden
