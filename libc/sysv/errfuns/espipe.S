#include "libc/macros.h"
.text.unlikely

espipe:	.leafprologue
	.profilable
	mov	ESPIPE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	espipe,globl,hidden
