#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eintr:	.leafprologue
	mov	EINTR(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eintr,globl,hidden
