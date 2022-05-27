#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

exfull:	.leafprologue
	mov	EXFULL(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	exfull,globl,hidden
