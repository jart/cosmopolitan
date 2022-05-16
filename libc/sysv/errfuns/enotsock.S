#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotsock:
	.leafprologue
	mov	ENOTSOCK(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotsock,globl,hidden
