#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

epipe:	.leafprologue
	mov	EPIPE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	epipe,globl,hidden
