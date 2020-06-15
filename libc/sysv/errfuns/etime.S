#include "libc/macros.h"
.text.unlikely

etime:	.leafprologue
	.profilable
	mov	ETIME(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etime,globl,hidden
