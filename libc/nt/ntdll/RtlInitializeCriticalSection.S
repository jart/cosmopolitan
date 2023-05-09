#include "libc/nt/ntdllimport.h"
.ntimp	RtlInitializeCriticalSection,RtlInitializeCriticalSection

	.text.windows
RtlInitializeCriticalSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlInitializeCriticalSection(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlInitializeCriticalSection,globl
	.previous
