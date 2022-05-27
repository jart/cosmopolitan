#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eownerdead:
	.leafprologue
	mov	EOWNERDEAD(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eownerdead,globl,hidden
