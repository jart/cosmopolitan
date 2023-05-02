#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapAlloc,HeapAlloc,0

#ifdef __x86_64__
	.text.windows
HeapAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapAlloc,globl
	.previous
#endif
