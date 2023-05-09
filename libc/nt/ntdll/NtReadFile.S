#include "libc/nt/ntdllimport.h"
.ntimp	NtReadFile,NtReadFile

	.text.windows
NtReadFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReadFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtReadFile,globl
	.previous
