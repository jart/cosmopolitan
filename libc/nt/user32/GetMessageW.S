#include "libc/nt/codegen.h"
.imp	user32,__imp_GetMessageW,GetMessageW

	.text.windows
	.ftrace1
GetMessage:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetMessageW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetMessage,globl
	.previous
