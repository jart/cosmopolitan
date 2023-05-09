#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateIoCompletion,NtCreateIoCompletion

	.text.windows
NtCreateIoCompletion:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateIoCompletion(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateIoCompletion,globl
	.previous
