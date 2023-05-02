#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LoadResource,LoadResource,0

#ifdef __x86_64__
	.text.windows
LoadResource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadResource(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadResource,globl
	.previous
#endif
