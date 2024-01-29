#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SuspendThread,SuspendThread

	.text.windows
	.ftrace1
SuspendThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SuspendThread(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	SuspendThread,globl
	.previous
