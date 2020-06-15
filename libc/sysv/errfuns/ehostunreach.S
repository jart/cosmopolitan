#include "libc/macros.h"
.text.unlikely

ehostunreach:
	.leafprologue
	.profilable
	mov	EHOSTUNREACH(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehostunreach,globl,hidden
