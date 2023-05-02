#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LockFileEx,LockFileEx,0

#ifdef __x86_64__
	.text.windows
__LockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__LockFileEx,globl
	.previous
#endif
