#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapCreate,HeapCreate,0

#ifdef __x86_64__
	.text.windows
HeapCreate:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapCreate(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapCreate,globl
	.previous
#endif
