#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadslt:
	.leafprologue
	mov	EBADSLT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadslt,globl,hidden
