#include "libc/macros.internal.h"
.text.unlikely

eunatch:
	.leafprologue
	.profilable
	mov	EUNATCH(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eunatch,globl,hidden
