#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualAllocEx,VirtualAllocEx,0

#ifdef __x86_64__
	.text.windows
VirtualAllocEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualAllocEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	VirtualAllocEx,globl
	.previous
#endif
