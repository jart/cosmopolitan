#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegCreateKeyExW,RegCreateKeyExW

	.text.windows
	.ftrace1
RegCreateKeyEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_RegCreateKeyExW(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegCreateKeyEx,globl
	.previous
