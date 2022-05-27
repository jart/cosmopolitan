#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

erange:	.leafprologue
	mov	ERANGE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erange,globl,hidden
