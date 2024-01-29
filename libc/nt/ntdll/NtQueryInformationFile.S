#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationFile,NtQueryInformationFile

	.text.windows
	.ftrace1
NtQueryInformationFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryInformationFile(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryInformationFile,globl
	.previous
