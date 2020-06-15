#include "libc/macros.h"
.text.unlikely

enxio:	.leafprologue
	.profilable
	mov	ENXIO(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enxio,globl,hidden
