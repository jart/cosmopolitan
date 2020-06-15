#include "libc/macros.h"
.text.unlikely

eafnosupport:
	.leafprologue
	.profilable
	mov	EAFNOSUPPORT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eafnosupport,globl,hidden
