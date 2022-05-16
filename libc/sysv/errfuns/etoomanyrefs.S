#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

etoomanyrefs:
	.leafprologue
	mov	ETOOMANYREFS(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etoomanyrefs,globl,hidden
