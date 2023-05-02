#include "libc/nt/ntdllimport.h"
.ntimp	NtReadVirtualMemory,NtReadVirtualMemory

#ifdef __x86_64__
	.text.windows
NtReadVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReadVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtReadVirtualMemory,globl
	.previous
#endif
