#include "libc/macros.internal.h"
.text.unlikely

enotconn:
	.leafprologue
	.profilable
	mov	ENOTCONN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotconn,globl,hidden
