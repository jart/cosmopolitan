#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GlobalMemoryStatusEx,GlobalMemoryStatusEx

	.text.windows
	.ftrace1
GlobalMemoryStatusEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GlobalMemoryStatusEx(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GlobalMemoryStatusEx,globl
	.previous
