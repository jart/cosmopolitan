#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eaddrinuse:
	.leafprologue
	mov	EADDRINUSE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eaddrinuse,globl,hidden
