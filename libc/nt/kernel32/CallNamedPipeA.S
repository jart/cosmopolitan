#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CallNamedPipeA,CallNamedPipeA,110

#ifdef __x86_64__
	.text.windows
CallNamedPipeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNamedPipeA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CallNamedPipeA,globl
	.previous
#endif
