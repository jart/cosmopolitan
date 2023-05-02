#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapFree,HeapFree,847

#ifdef __x86_64__
	.text.windows
HeapFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapFree(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapFree,globl
	.previous
#endif
