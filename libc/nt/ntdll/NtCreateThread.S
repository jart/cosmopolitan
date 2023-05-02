#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateThread,NtCreateThread

#ifdef __x86_64__
	.text.windows
NtCreateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateThread(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateThread,globl
	.previous
#endif
