#include "libc/macros.h"
.text.unlikely

econnaborted:
	.leafprologue
	.profilable
	mov	ECONNABORTED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnaborted,globl,hidden
