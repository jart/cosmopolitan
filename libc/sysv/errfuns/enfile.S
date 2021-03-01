#include "libc/macros.internal.h"
.text.unlikely

enfile:	.leafprologue
	.profilable
	mov	ENFILE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enfile,globl,hidden
