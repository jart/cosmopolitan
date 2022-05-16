#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enotnam:
	.leafprologue
	mov	ENOTNAM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enotnam,globl,hidden
