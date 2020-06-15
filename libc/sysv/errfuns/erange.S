#include "libc/macros.h"
.text.unlikely

erange:	.leafprologue
	.profilable
	mov	ERANGE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erange,globl,hidden
