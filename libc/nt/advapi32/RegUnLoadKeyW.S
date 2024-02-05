#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegUnLoadKeyW,RegUnLoadKeyW

	.text.windows
	.ftrace1
RegUnLoadKey:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegUnLoadKeyW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegUnLoadKey,globl
	.previous
