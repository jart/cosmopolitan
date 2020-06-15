#include "libc/macros.h"
.text.unlikely

eacces:	.leafprologue
	.profilable
	mov	EACCES(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eacces,globl,hidden
