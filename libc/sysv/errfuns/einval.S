#include "libc/macros.internal.h"
.text.unlikely

einval:	.leafprologue
	.profilable
	mov	EINVAL(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	einval,globl,hidden
