#include "libc/nt/ntdllimport.h"
.ntimp	NtFsControlFile,NtFsControlFile

	.text.windows
NtFsControlFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFsControlFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtFsControlFile,globl
	.previous
