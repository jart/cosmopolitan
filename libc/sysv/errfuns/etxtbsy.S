#include "libc/macros.h"
.text.unlikely

etxtbsy:
	.leafprologue
	.profilable
	mov	ETXTBSY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etxtbsy,globl,hidden
