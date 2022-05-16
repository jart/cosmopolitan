#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eio:	.leafprologue
	mov	EIO(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eio,globl,hidden
