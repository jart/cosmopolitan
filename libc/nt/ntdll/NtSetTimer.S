#include "libc/nt/ntdllimport.h"
.ntimp	NtSetTimer,NtSetTimer

	.text.windows
NtSetTimer:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetTimer(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetTimer,globl
	.previous
