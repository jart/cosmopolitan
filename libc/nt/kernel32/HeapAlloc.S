#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapAlloc,HeapAlloc,0

	.text.windows
HeapAlloc:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapAlloc(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	HeapAlloc,globl
	.previous
