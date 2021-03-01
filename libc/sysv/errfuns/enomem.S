#include "libc/macros.internal.h"
.text.unlikely

enomem:	.leafprologue
	.profilable
	mov	ENOMEM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomem,globl,hidden
