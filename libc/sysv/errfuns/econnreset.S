#include "libc/macros.internal.h"
.text.unlikely

econnreset:
	.leafprologue
	.profilable
	mov	ECONNRESET(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnreset,globl,hidden
