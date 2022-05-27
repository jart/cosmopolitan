#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadfd:	.leafprologue
	mov	EBADFD(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadfd,globl,hidden
