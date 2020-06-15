#include "libc/macros.h"
.text.unlikely

emsgsize:
	.leafprologue
	.profilable
	mov	EMSGSIZE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emsgsize,globl,hidden
