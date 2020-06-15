#include "libc/macros.h"
.text.unlikely

enavail:
	.leafprologue
	.profilable
	mov	ENAVAIL(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enavail,globl,hidden
