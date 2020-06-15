#include "libc/macros.h"
.text.unlikely

emediumtype:
	.leafprologue
	.profilable
	mov	EMEDIUMTYPE(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emediumtype,globl,hidden
