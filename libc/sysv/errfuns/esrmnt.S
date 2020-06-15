#include "libc/macros.h"
.text.unlikely

esrmnt:	.leafprologue
	.profilable
	mov	ESRMNT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	esrmnt,globl,hidden
