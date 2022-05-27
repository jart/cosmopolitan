#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

etime:	.leafprologue
	mov	ETIME(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etime,globl,hidden
