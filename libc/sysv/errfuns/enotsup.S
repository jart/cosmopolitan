#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotsup:
	.leafprologue
	mov	ENOTSUP(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotsup,globl,hidden
