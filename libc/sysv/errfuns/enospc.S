#include "libc/macros.h"
.text.unlikely

enospc:	.leafprologue
	.profilable
	mov	ENOSPC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enospc,globl,hidden
