#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

el3rst:	.leafprologue
	mov	EL3RST(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el3rst,globl,hidden
