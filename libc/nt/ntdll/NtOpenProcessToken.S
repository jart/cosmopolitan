#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenProcessToken,NtOpenProcessToken

	.text.windows
	.ftrace1
NtOpenProcessToken:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenProcessToken(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenProcessToken,globl
	.previous
