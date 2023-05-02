#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenProcessToken,NtOpenProcessToken

#ifdef __x86_64__
	.text.windows
NtOpenProcessToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenProcessToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenProcessToken,globl
	.previous
#endif
