#include "libc/macros.h"
.text.unlikely

eilseq:	.leafprologue
	.profilable
	mov	EILSEQ(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eilseq,globl,hidden
