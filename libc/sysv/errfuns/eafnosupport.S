#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eafnosupport:
	.leafprologue
	mov	EAFNOSUPPORT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eafnosupport,globl,hidden
