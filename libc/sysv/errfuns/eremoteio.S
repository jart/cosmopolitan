#include "libc/macros.h"
.text.unlikely

eremoteio:
	.leafprologue
	.profilable
	mov	EREMOTEIO(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremoteio,globl,hidden
