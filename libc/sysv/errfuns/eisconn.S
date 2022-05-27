#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eisconn:
	.leafprologue
	mov	EISCONN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eisconn,globl,hidden
