#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSetValueExW,RegSetValueExW

	.text.windows
	.ftrace1
RegSetValueEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegSetValueExW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegSetValueEx,globl
	.previous
