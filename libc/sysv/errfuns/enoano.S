#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enoano:	.leafprologue
	mov	ENOANO(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoano,globl,hidden
