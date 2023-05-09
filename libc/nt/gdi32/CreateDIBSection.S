#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateDIBSection,CreateDIBSection,1069

	.text.windows
CreateDIBSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDIBSection(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateDIBSection,globl
	.previous
