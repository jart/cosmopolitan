#include "libc/nt/ntdllimport.h"
.ntimp	NtAllocateVirtualMemory,NtAllocateVirtualMemory

#ifdef __x86_64__
	.text.windows
NtAllocateVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtAllocateVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtAllocateVirtualMemory,globl
	.previous
#endif
