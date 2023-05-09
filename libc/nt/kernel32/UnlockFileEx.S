#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UnlockFileEx,UnlockFileEx,0

	.text.windows
__UnlockFileEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFileEx(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__UnlockFileEx,globl
	.previous
