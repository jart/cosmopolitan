#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualAlloc,VirtualAlloc,0

#ifdef __x86_64__
	.text.windows
VirtualAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualAlloc,globl
	.previous
#endif
