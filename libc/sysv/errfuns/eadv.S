#include "libc/macros.h"
.text.unlikely

eadv:	.leafprologue
	.profilable
	mov	EADV(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eadv,globl,hidden
