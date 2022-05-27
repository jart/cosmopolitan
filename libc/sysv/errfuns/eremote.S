#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eremote:
	.leafprologue
	mov	EREMOTE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eremote,globl,hidden
