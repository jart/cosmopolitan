#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WaitForMultipleObjectsEx,WaitForMultipleObjectsEx,0

#ifdef __x86_64__
	.text.windows
WaitForMultipleObjectsEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForMultipleObjectsEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WaitForMultipleObjectsEx,globl
	.previous
#endif
