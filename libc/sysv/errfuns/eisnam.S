#include "libc/macros.h"
.text.unlikely

eisnam:	.leafprologue
	.profilable
	mov	EISNAM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eisnam,globl,hidden
