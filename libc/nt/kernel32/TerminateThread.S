#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TerminateThread,TerminateThread,0

#ifdef __x86_64__
	.text.windows
TerminateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TerminateThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	TerminateThread,globl
	.previous
#endif
