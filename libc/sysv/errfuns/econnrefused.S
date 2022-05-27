#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

econnrefused:
	.leafprologue
	mov	ECONNREFUSED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnrefused,globl,hidden
