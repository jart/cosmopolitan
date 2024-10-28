#include "libc/nt/codegen.h"
.imp	user32,__imp_GetRawInputData,GetRawInputData

	.text.windows
	.ftrace1
GetRawInputData:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetRawInputData(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetRawInputData,globl
	.previous
