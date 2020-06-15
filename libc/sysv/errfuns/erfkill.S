#include "libc/macros.h"
.text.unlikely

erfkill:
	.leafprologue
	.profilable
	mov	ERFKILL(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erfkill,globl,hidden
