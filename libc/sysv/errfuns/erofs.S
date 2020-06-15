#include "libc/macros.h"
.text.unlikely

erofs:	.leafprologue
	.profilable
	mov	EROFS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erofs,globl,hidden
