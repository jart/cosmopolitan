#include "libc/nt/ntdllimport.h"
.ntimp	NtWriteFile,NtWriteFile

	.text.windows
	.ftrace1
NtWriteFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtWriteFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtWriteFile,globl
	.previous
