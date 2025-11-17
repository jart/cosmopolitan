#include "libc/nt/codegen.h"
.imp	advapi32,__imp_AddAce,AddAce

	.text.windows
	.ftrace1
AddAce:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_AddAce(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AddAce,globl
	.previous
