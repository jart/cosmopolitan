#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

econnreset:
	.leafprologue
	mov	ECONNRESET(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	econnreset,globl,hidden
