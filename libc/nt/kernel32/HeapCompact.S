#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapCompact,HeapCompact,0

#ifdef __x86_64__
	.text.windows
HeapCompact:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapCompact(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapCompact,globl
	.previous
#endif
