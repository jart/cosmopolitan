#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushVirtualMemory,NtFlushVirtualMemory

	.text.windows
	.ftrace1
NtFlushVirtualMemory:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtFlushVirtualMemory(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFlushVirtualMemory,globl
	.previous
