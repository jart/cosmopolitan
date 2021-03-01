#include "libc/macros.internal.h"
.text.unlikely

eprotonosupport:
	.leafprologue
	.profilable
	mov	EPROTONOSUPPORT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eprotonosupport,globl,hidden
