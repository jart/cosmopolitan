#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateFile,NtCreateFile

	.text.windows
	.ftrace1
NtCreateFile:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateFile(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateFile,globl
	.previous
