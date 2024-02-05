#include "libc/nt/codegen.h"
.imp	advapi32,__imp_DuplicateTokenEx,DuplicateTokenEx

	.text.windows
	.ftrace1
DuplicateTokenEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_DuplicateTokenEx(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DuplicateTokenEx,globl
	.previous
