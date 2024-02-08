#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationThread,NtQueryInformationThread

	.text.windows
	.ftrace1
NtQueryInformationThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryInformationThread(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryInformationThread,globl
	.previous
