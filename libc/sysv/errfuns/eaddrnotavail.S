#include "libc/macros.h"
.text.unlikely

eaddrnotavail:
	.leafprologue
	.profilable
	mov	EADDRNOTAVAIL(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eaddrnotavail,globl,hidden
