#include "libc/macros.h"
.text.unlikely

enoprotoopt:
	.leafprologue
	.profilable
	mov	ENOPROTOOPT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoprotoopt,globl,hidden
