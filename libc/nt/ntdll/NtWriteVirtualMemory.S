#include "libc/nt/ntdllimport.h"
.ntimp	NtWriteVirtualMemory,NtWriteVirtualMemory

#ifdef __x86_64__
	.text.windows
NtWriteVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWriteVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtWriteVirtualMemory,globl
	.previous
#endif
