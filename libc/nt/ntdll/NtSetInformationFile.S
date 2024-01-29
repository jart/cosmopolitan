#include "libc/nt/ntdllimport.h"
.ntimp	NtSetInformationFile,NtSetInformationFile

	.text.windows
	.ftrace1
NtSetInformationFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtSetInformationFile(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetInformationFile,globl
	.previous
