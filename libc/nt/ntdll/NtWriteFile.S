#include "libc/nt/ntdllimport.h"
.ntimp	NtWriteFile,NtWriteFile

	.text.windows
NtWriteFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWriteFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtWriteFile,globl
	.previous
