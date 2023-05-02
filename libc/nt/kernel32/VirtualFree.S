#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualFree,VirtualFree,0

#ifdef __x86_64__
	.text.windows
VirtualFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualFree(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualFree,globl
	.previous
#endif
