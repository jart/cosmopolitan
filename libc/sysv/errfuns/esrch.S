#include "libc/macros.internal.h"
.text.unlikely

esrch:	.leafprologue
	.profilable
	mov	ESRCH(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	esrch,globl,hidden
