#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WaitForSingleObjectEx,WaitForSingleObjectEx,0

#ifdef __x86_64__
	.text.windows
WaitForSingleObjectEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObjectEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForSingleObjectEx,globl
	.previous
#endif
