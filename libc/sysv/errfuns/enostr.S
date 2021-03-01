#include "libc/macros.internal.h"
.text.unlikely

enostr:	.leafprologue
	.profilable
	mov	ENOSTR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enostr,globl,hidden
