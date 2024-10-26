#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateWindowW,CreateWindowW

	.text.windows
	.ftrace1
CreateWindow:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateWindowW(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateWindow,globl
	.previous
