#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UnlockFileEx,UnlockFileEx,0

#ifdef __x86_64__
	.text.windows
__UnlockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__UnlockFileEx,globl
	.previous
#endif
