#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eremchg:
	.leafprologue
	mov	EREMCHG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremchg,globl,hidden
