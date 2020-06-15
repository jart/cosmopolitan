#include "libc/macros.h"
.text.unlikely

enolck:	.leafprologue
	.profilable
	mov	ENOLCK(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enolck,globl,hidden
