#include "libc/macros.h"
.text.unlikely

estrpipe:
	.leafprologue
	.profilable
	mov	ESTRPIPE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	estrpipe,globl,hidden
