#include "libc/nt/ntdllimport.h"
.ntimp	NtFreeVirtualMemory,NtFreeVirtualMemory

#ifdef __x86_64__
	.text.windows
NtFreeVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFreeVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFreeVirtualMemory,globl
	.previous
#endif
