#include "libc/macros.h"
.text.unlikely

enopkg:	.leafprologue
	.profilable
	mov	ENOPKG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enopkg,globl,hidden
