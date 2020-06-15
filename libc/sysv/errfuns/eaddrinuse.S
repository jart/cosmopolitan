#include "libc/macros.h"
.text.unlikely

eaddrinuse:
	.leafprologue
	.profilable
	mov	EADDRINUSE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eaddrinuse,globl,hidden
