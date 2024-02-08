#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryDirectoryFile,NtQueryDirectoryFile

	.text.windows
	.ftrace1
NtQueryDirectoryFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryDirectoryFile(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryDirectoryFile,globl
	.previous
