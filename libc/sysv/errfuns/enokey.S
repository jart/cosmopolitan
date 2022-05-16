#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enokey:	.leafprologue
	mov	ENOKEY(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enokey,globl,hidden
