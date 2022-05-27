#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enetdown:
	.leafprologue
	mov	ENETDOWN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enetdown,globl,hidden
