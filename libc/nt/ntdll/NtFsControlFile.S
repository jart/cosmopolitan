#include "libc/nt/ntdllimport.h"
.ntimp	NtFsControlFile,NtFsControlFile

	.text.windows
	.ftrace1
NtFsControlFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtFsControlFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFsControlFile,globl
	.previous
