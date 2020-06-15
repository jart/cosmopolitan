#include "libc/macros.h"
.text.unlikely

eremote:
	.leafprologue
	.profilable
	mov	EREMOTE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremote,globl,hidden
