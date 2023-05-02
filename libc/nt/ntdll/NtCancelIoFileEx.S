#include "libc/nt/ntdllimport.h"
.ntimp	NtCancelIoFileEx,NtCancelIoFileEx

#ifdef __x86_64__
	.text.windows
NtCancelIoFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFileEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCancelIoFileEx,globl
	.previous
#endif
