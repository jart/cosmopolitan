#include "libc/macros.h"
.text.unlikely

ehostdown:
	.leafprologue
	.profilable
	mov	EHOSTDOWN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehostdown,globl,hidden
