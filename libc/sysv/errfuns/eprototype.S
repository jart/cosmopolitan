#include "libc/macros.h"
.text.unlikely

eprototype:
	.leafprologue
	.profilable
	mov	EPROTOTYPE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eprototype,globl,hidden
