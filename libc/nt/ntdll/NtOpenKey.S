#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenKey,NtOpenKey

	.text.windows
NtOpenKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenKey(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenKey,globl
	.previous
