#include "libc/nt/codegen.h"
.imp	advapi32,__imp_InitiateShutdownW,InitiateShutdownW,1403

#ifdef __x86_64__
	.text.windows
InitiateShutdown:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitiateShutdownW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	InitiateShutdown,globl
	.previous
#endif
