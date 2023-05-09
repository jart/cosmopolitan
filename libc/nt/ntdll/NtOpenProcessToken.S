#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenProcessToken,NtOpenProcessToken

	.text.windows
NtOpenProcessToken:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenProcessToken(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenProcessToken,globl
	.previous
