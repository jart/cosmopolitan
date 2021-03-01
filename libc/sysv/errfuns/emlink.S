#include "libc/macros.internal.h"
.text.unlikely

emlink:	.leafprologue
	.profilable
	mov	EMLINK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emlink,globl,hidden
