#include "libc/macros.h"
.text.unlikely

enotdir:
	.leafprologue
	.profilable
	mov	ENOTDIR(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotdir,globl,hidden
