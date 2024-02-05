#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenFile,NtOpenFile

	.text.windows
	.ftrace1
NtOpenFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenFile(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenFile,globl
	.previous
