#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enomedium:
	.leafprologue
	mov	ENOMEDIUM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enomedium,globl,hidden
