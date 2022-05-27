#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enavail:
	.leafprologue
	mov	ENAVAIL(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enavail,globl,hidden
