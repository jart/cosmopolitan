#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

emsgsize:
	.leafprologue
	mov	EMSGSIZE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emsgsize,globl,hidden
