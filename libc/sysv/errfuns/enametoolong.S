#include "libc/macros.h"
.text.unlikely

enametoolong:
	.leafprologue
	.profilable
	mov	ENAMETOOLONG(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enametoolong,globl,hidden
