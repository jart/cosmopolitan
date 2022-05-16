#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotuniq:
	.leafprologue
	mov	ENOTUNIQ(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotuniq,globl,hidden
