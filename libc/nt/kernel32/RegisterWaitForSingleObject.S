#include "libc/nt/codegen.h"
.imp	kernel32,__imp_RegisterWaitForSingleObject,RegisterWaitForSingleObject,1191

	.text.windows
RegisterWaitForSingleObject:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegisterWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegisterWaitForSingleObject,globl
	.previous
