#include "libc/macros.internal.h"
.text.unlikely

el3rst:	.leafprologue
	.profilable
	mov	EL3RST(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el3rst,globl,hidden
