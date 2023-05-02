#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapReAlloc,HeapReAlloc,0

#ifdef __x86_64__
	.text.windows
HeapReAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapReAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapReAlloc,globl
	.previous
#endif
