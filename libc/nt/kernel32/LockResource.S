#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LockResource,LockResource

	.text.windows
	.ftrace1
LockResource:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LockResource(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	LockResource,globl
	.previous
