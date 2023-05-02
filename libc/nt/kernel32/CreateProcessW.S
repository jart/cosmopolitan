#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateProcessW,CreateProcessW,0

#ifdef __x86_64__
	.text.windows
__CreateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	__CreateProcess,globl
	.previous
#endif
