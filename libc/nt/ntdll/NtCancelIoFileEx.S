#include "libc/nt/ntdllimport.h"
.ntimp	NtCancelIoFileEx,NtCancelIoFileEx

	.text.windows
NtCancelIoFileEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFileEx(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCancelIoFileEx,globl
	.previous
