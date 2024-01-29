#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteValueW,RegDeleteValueW

	.text.windows
	.ftrace1
RegDeleteValue:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegDeleteValueW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegDeleteValue,globl
	.previous
