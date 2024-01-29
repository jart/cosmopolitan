#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegGetValueW,RegGetValueW

	.text.windows
	.ftrace1
RegGetValue:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegGetValueW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegGetValue,globl
	.previous
