#include "libc/macros.h"
.text.unlikely

enetunreach:
	.leafprologue
	.profilable
	mov	ENETUNREACH(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enetunreach,globl,hidden
