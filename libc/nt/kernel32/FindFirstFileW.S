#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindFirstFileW,FindFirstFileW,0

	.text.windows
__FindFirstFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__FindFirstFile,globl
	.previous
