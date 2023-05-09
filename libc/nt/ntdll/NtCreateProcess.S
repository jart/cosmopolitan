#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateProcess,NtCreateProcess

	.text.windows
NtCreateProcess:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateProcess(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateProcess,globl
	.previous
