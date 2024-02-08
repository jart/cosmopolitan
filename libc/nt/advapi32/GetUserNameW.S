#include "libc/nt/codegen.h"
.imp	advapi32,__imp_GetUserNameW,GetUserNameW

	.text.windows
	.ftrace1
GetUserName:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetUserNameW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetUserName,globl
	.previous
