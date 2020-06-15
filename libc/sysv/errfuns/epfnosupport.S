#include "libc/macros.h"
.text.unlikely

epfnosupport:
	.leafprologue
	.profilable
	mov	EPFNOSUPPORT(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	epfnosupport,globl,hidden
