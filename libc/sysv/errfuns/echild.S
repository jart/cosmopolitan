#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

echild:	.leafprologue
	mov	ECHILD(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	echild,globl,hidden
