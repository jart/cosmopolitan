#include "libc/nt/ntdllimport.h"
.ntimp	NtAllocateVirtualMemory,NtAllocateVirtualMemory

	.text.windows
	.ftrace1
NtAllocateVirtualMemory:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtAllocateVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtAllocateVirtualMemory,globl
	.previous
