#include "libc/nt/codegen.h"
.imp	MsWSock,__imp_AcceptEx,AcceptEx,0

#ifdef __x86_64__
	.text.windows
AcceptEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AcceptEx(%rip),%rax
	jmp	__sysv2nt8
	.endfn	AcceptEx,globl
	.previous
#endif
