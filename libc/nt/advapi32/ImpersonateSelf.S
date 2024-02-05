#include "libc/nt/codegen.h"
.imp	advapi32,__imp_ImpersonateSelf,ImpersonateSelf

	.text.windows
	.ftrace1
ImpersonateSelf:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ImpersonateSelf(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	ImpersonateSelf,globl
	.previous
