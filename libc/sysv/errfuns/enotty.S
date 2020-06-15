#include "libc/macros.h"
.text.unlikely

enotty:	.leafprologue
	.profilable
	mov	ENOTTY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotty,globl,hidden
