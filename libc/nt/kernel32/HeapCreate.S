#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapCreate,HeapCreate,0

	.text.windows
HeapCreate:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapCreate(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	HeapCreate,globl
	.previous
