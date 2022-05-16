#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eunatch:
	.leafprologue
	mov	EUNATCH(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eunatch,globl,hidden
