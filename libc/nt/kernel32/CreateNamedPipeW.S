#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateNamedPipeW,CreateNamedPipeW,0

#ifdef __x86_64__
	.text.windows
__CreateNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	__CreateNamedPipe,globl
	.previous
#endif
