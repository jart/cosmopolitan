#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eusers:	.leafprologue
	mov	EUSERS(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eusers,globl,hidden
