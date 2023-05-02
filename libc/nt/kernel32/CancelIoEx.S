#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CancelIoEx,CancelIoEx,0

#ifdef __x86_64__
	.text.windows
CancelIoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CancelIoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	CancelIoEx,globl
	.previous
#endif
