#include "libc/macros.h"
.text.unlikely

eshutdown:
	.leafprologue
	.profilable
	mov	ESHUTDOWN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eshutdown,globl,hidden
