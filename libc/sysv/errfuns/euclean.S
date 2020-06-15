#include "libc/macros.h"
.text.unlikely

euclean:
	.leafprologue
	.profilable
	mov	EUCLEAN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	euclean,globl,hidden
