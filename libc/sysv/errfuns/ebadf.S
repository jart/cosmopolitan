#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadf:	.leafprologue
	mov	EBADF(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadf,globl,hidden
