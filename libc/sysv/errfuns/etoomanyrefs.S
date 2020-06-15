#include "libc/macros.h"
.text.unlikely

etoomanyrefs:
	.leafprologue
	.profilable
	mov	ETOOMANYREFS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etoomanyrefs,globl,hidden
