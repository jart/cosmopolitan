#include "libc/nt/codegen.h"
.imp	kernel32,__imp_Process32NextW,Process32NextW

	.text.windows
	.ftrace1
Process32Next:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_Process32NextW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	Process32Next,globl
	.previous
