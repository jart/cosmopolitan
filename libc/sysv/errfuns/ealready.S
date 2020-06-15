#include "libc/macros.h"
.text.unlikely

ealready:
	.leafprologue
	.profilable
	mov	EALREADY(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ealready,globl,hidden
