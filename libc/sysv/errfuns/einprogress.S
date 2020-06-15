#include "libc/macros.h"
.text.unlikely

einprogress:
	.leafprologue
	.profilable
	mov	EINPROGRESS(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	einprogress,globl,hidden
