#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ekeyrejected:
	.leafprologue
	mov	EKEYREJECTED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyrejected,globl,hidden
