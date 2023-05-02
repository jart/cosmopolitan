#include "libc/nt/codegen.h"
.imp	kernel32,__imp_RegisterWaitForSingleObject,RegisterWaitForSingleObject,1191

#ifdef __x86_64__
	.text.windows
RegisterWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegisterWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegisterWaitForSingleObject,globl
	.previous
#endif
