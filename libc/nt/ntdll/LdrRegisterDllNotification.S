#include "libc/nt/ntdllimport.h"
.ntimp	LdrRegisterDllNotification,LdrRegisterDllNotification

	.text.windows
	.ftrace1
LdrRegisterDllNotification:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_LdrRegisterDllNotification(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LdrRegisterDllNotification,globl
	.previous
