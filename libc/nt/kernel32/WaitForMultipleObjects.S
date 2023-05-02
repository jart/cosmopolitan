#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WaitForMultipleObjects,WaitForMultipleObjects,0

#ifdef __x86_64__
	.text.windows
__WaitForMultipleObjects:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForMultipleObjects(%rip),%rax
	jmp	__sysv2nt
	.endfn	__WaitForMultipleObjects,globl
	.previous
#endif
