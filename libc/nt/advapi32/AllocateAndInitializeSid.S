#include "libc/nt/codegen.h"
.imp	advapi32,__imp_AllocateAndInitializeSid,AllocateAndInitializeSid

	.text.windows
	.ftrace1
AllocateAndInitializeSid:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_AllocateAndInitializeSid(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AllocateAndInitializeSid,globl
	.previous
