#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

exdev:	.leafprologue
	mov	EXDEV(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	exdev,globl,hidden
