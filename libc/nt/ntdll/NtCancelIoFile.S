#include "libc/nt/ntdllimport.h"
.ntimp	NtCancelIoFile,NtCancelIoFile

	.text.windows
NtCancelIoFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCancelIoFile,globl
	.previous