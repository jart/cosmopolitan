#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateIoCompletion,NtCreateIoCompletion

#ifdef __x86_64__
	.text.windows
NtCreateIoCompletion:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateIoCompletion(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateIoCompletion,globl
	.previous
#endif
