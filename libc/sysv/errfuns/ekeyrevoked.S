#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ekeyrevoked:
	.leafprologue
	mov	EKEYREVOKED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyrevoked,globl,hidden
