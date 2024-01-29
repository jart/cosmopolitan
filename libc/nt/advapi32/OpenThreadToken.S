#include "libc/nt/codegen.h"
.imp	advapi32,__imp_OpenThreadToken,OpenThreadToken

	.text.windows
	.ftrace1
OpenThreadToken:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_OpenThreadToken(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	OpenThreadToken,globl
	.previous
