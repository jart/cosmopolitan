#include "libc/nt/ntdllimport.h"
.ntimp	NtFreeVirtualMemory,NtFreeVirtualMemory

	.text.windows
	.ftrace1
NtFreeVirtualMemory:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtFreeVirtualMemory(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFreeVirtualMemory,globl
	.previous
