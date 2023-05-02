#include "libc/nt/ntdllimport.h"
.ntimp	NtProtectVirtualMemory,NtProtectVirtualMemory

#ifdef __x86_64__
	.text.windows
NtProtectVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtProtectVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtProtectVirtualMemory,globl
	.previous
#endif
