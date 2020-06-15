#include "libc/macros.h"
.text.unlikely

econnrefused:
	.leafprologue
	.profilable
	mov	ECONNREFUSED(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnrefused,globl,hidden
