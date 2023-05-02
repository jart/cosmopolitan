#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenThread,NtOpenThread

#ifdef __x86_64__
	.text.windows
NtOpenThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenThread,globl
	.previous
#endif
