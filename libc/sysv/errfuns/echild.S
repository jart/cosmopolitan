#include "libc/macros.h"
.text.unlikely

echild:	.leafprologue
	.profilable
	mov	ECHILD(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	echild,globl,hidden
