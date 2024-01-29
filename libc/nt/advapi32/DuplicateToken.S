#include "libc/nt/codegen.h"
.imp	advapi32,__imp_DuplicateToken,DuplicateToken

	.text.windows
	.ftrace1
DuplicateToken:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_DuplicateToken(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DuplicateToken,globl
	.previous
