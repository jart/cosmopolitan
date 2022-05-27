#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ehostunreach:
	.leafprologue
	mov	EHOSTUNREACH(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ehostunreach,globl,hidden
