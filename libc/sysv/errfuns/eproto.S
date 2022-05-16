#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eproto:	.leafprologue
	mov	EPROTO(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eproto,globl,hidden
