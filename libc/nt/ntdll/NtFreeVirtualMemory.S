#include "libc/nt/ntdllimport.h"
.ntimp	NtFreeVirtualMemory,NtFreeVirtualMemory

	.text.windows
NtFreeVirtualMemory:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFreeVirtualMemory(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFreeVirtualMemory,globl
	.previous
