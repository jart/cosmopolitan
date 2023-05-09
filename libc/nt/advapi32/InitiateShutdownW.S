#include "libc/nt/codegen.h"
.imp	advapi32,__imp_InitiateShutdownW,InitiateShutdownW,1403

	.text.windows
InitiateShutdown:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitiateShutdownW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	InitiateShutdown,globl
	.previous
