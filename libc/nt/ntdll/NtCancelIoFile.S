#include "libc/nt/ntdllimport.h"
.ntimp	NtCancelIoFile,NtCancelIoFile

#ifdef __x86_64__
	.text.windows
NtCancelIoFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCancelIoFile,globl
	.previous
#endif
