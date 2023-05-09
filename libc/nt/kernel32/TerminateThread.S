#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TerminateThread,TerminateThread,0

	.text.windows
TerminateThread:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TerminateThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	TerminateThread,globl
	.previous
