#include "libc/macros.h"
.text.unlikely

emfile:	.leafprologue
	.profilable
	mov	EMFILE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emfile,globl,hidden
