#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegEnableReflectionKey,RegEnableReflectionKey

	.text.windows
	.ftrace1
RegEnableReflectionKey:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegEnableReflectionKey(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RegEnableReflectionKey,globl
	.previous
