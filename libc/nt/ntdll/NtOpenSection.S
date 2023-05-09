#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenSection,NtOpenSection

	.text.windows
NtOpenSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenSection(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenSection,globl
	.previous
