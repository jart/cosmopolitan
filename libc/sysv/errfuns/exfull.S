#include "libc/macros.h"
.text.unlikely

exfull:	.leafprologue
	.profilable
	mov	EXFULL(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	exfull,globl,hidden
