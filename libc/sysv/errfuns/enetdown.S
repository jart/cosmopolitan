#include "libc/macros.h"
.text.unlikely

enetdown:
	.leafprologue
	.profilable
	mov	ENETDOWN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enetdown,globl,hidden
