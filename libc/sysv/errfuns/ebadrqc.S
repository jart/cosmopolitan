#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadrqc:
	.leafprologue
	mov	EBADRQC(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadrqc,globl,hidden
