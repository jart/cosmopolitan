#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LockFileEx,LockFileEx

	.text.windows
        .ftrace1
__LockFileEx:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_LockFileEx(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__LockFileEx,globl
	.previous
