#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationJobObject,NtQueryInformationJobObject

	.text.windows
	.ftrace1
NtQueryInformationJobObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryInformationJobObject(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryInformationJobObject,globl
	.previous
