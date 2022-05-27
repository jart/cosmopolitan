#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ehwpoison:
	.leafprologue
	mov	EHWPOISON(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehwpoison,globl,hidden
