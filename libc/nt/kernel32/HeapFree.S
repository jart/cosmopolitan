#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapFree,HeapFree,847

	.text.windows
HeapFree:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapFree(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	HeapFree,globl
	.previous
