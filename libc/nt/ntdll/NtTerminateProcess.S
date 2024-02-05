#include "libc/nt/ntdllimport.h"
.ntimp	NtTerminateProcess,NtTerminateProcess

	.text.windows
	.ftrace1
NtTerminateProcess:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtTerminateProcess(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtTerminateProcess,globl
	.previous
