#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenThreadToken,NtOpenThreadToken

#ifdef __x86_64__
	.text.windows
NtOpenThreadToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenThreadToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenThreadToken,globl
	.previous
#endif
