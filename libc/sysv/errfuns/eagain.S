#include "libc/macros.h"
.text.unlikely

eagain:	.leafprologue
	.profilable
	mov	EAGAIN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eagain,globl,hidden
