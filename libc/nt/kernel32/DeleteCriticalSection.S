#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DeleteCriticalSection,DeleteCriticalSection

	.text.windows
	.ftrace1
DeleteCriticalSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteCriticalSection(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	DeleteCriticalSection,globl
	.previous
