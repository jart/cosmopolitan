#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushVirtualMemory,NtFlushVirtualMemory

#ifdef __x86_64__
	.text.windows
NtFlushVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushVirtualMemory,globl
	.previous
#endif
