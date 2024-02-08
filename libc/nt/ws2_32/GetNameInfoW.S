#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetNameInfoW,GetNameInfoW

	.text.windows
	.ftrace1
GetNameInfo:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetNameInfoW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetNameInfo,globl
	.previous
