#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotblk:
	.leafprologue
	mov	ENOTBLK(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotblk,globl,hidden
