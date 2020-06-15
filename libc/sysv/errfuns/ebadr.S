#include "libc/macros.h"
.text.unlikely

ebadr:	.leafprologue
	.profilable
	mov	EBADR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadr,globl,hidden
