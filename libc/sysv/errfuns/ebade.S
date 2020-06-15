#include "libc/macros.h"
.text.unlikely

ebade:	.leafprologue
	.profilable
	mov	EBADE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebade,globl,hidden
