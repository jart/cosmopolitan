#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enolink:
	.leafprologue
	mov	ENOLINK(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enolink,globl,hidden
