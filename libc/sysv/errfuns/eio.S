#include "libc/macros.h"
.text.unlikely

eio:	.leafprologue
	.profilable
	mov	EIO(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eio,globl,hidden
