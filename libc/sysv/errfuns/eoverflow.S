#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eoverflow:
	.leafprologue
	mov	EOVERFLOW(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eoverflow,globl,hidden
