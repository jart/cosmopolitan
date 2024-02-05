#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationProcess,NtQueryInformationProcess

	.text.windows
	.ftrace1
NtQueryInformationProcess:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryInformationProcess(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryInformationProcess,globl
	.previous
