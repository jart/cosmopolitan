#include "libc/macros.h"
.text.unlikely

enotnam:
	.leafprologue
	.profilable
	mov	ENOTNAM(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotnam,globl,hidden
