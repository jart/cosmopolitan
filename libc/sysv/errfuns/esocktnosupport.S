#include "libc/macros.h"
.text.unlikely

esocktnosupport:
	.leafprologue
	.profilable
	mov	ESOCKTNOSUPPORT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	esocktnosupport,globl,hidden
