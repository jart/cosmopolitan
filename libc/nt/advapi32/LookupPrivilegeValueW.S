#include "libc/nt/codegen.h"
.imp	advapi32,__imp_LookupPrivilegeValueW,LookupPrivilegeValueW

	.text.windows
	.ftrace1
LookupPrivilegeValue:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_LookupPrivilegeValueW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LookupPrivilegeValue,globl
	.previous
