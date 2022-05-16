#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enoent:	.leafprologue
	mov	ENOENT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoent,globl,hidden
