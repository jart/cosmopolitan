#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WaitForSingleObject,WaitForSingleObject,0

	.text.windows
__WaitForSingleObject:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__WaitForSingleObject,globl
	.previous
