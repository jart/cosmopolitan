#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateProcessW,CreateProcessW

	.text.windows
        .ftrace1
__CreateProcess:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateProcessW(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateProcess,globl
	.previous
