#include "libc/macros.h"
.text.unlikely

elibscn:
	.leafprologue
	.profilable
	mov	ELIBSCN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibscn,globl,hidden
