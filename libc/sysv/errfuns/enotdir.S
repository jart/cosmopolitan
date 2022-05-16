#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotdir:
	.leafprologue
	mov	ENOTDIR(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotdir,globl,hidden
