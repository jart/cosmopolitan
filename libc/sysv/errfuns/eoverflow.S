#include "libc/macros.h"
.text.unlikely

eoverflow:
	.leafprologue
	.profilable
	mov	EOVERFLOW(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eoverflow,globl,hidden
