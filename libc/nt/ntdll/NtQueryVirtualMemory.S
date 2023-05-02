#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryVirtualMemory,NtQueryVirtualMemory

#ifdef __x86_64__
	.text.windows
NtQueryVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryVirtualMemory,globl
	.previous
#endif
