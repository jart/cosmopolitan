#include "libc/nt/ntdllimport.h"
.ntimp	ZwAreMappedFilesTheSame,ZwAreMappedFilesTheSame

	.text.windows
ZwAreMappedFilesTheSame:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ZwAreMappedFilesTheSame(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ZwAreMappedFilesTheSame,globl
	.previous
