#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enetunreach:
	.leafprologue
	mov	ENETUNREACH(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enetunreach,globl,hidden
