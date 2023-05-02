#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CallNamedPipeW,CallNamedPipeW,0

#ifdef __x86_64__
	.text.windows
CallNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CallNamedPipe,globl
	.previous
#endif
