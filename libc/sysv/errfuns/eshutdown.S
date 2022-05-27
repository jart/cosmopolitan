#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eshutdown:
	.leafprologue
	mov	ESHUTDOWN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eshutdown,globl,hidden
