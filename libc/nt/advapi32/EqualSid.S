#include "libc/nt/codegen.h"
.imp	advapi32,__imp_EqualSid,EqualSid

	.text.windows
	.ftrace1
EqualSid:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_EqualSid(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	EqualSid,globl
	.previous
