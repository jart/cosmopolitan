#include "libc/macros.h"
.text.unlikely

enocsi:	.leafprologue
	.profilable
	mov	ENOCSI(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enocsi,globl,hidden
