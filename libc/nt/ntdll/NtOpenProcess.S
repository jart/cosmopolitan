#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenProcess,NtOpenProcess

	.text.windows
	.ftrace1
NtOpenProcess:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenProcess(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenProcess,globl
	.previous
