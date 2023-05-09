#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CallNamedPipeW,CallNamedPipeW,0

	.text.windows
CallNamedPipe:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CallNamedPipe,globl
	.previous
