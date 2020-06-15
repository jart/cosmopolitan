#include "libc/macros.h"
.text.unlikely

eisdir:	.leafprologue
	.profilable
	mov	EISDIR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eisdir,globl,hidden
