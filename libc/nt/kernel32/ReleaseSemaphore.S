#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReleaseSemaphore,ReleaseSemaphore,0

#ifdef __x86_64__
	.text.windows
ReleaseSemaphore:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReleaseSemaphore(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReleaseSemaphore,globl
	.previous
#endif
