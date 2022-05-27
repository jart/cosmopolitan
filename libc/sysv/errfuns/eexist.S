#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eexist:	.leafprologue
	mov	EEXIST(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eexist,globl,hidden
