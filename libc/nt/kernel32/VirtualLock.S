#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualLock,VirtualLock,0

#ifdef __x86_64__
	.text.windows
VirtualLock:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualLock(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualLock,globl
	.previous
#endif
