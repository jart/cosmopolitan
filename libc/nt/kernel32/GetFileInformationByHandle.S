#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileInformationByHandle,GetFileInformationByHandle,0

	.text.windows
GetFileInformationByHandle:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileInformationByHandle(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetFileInformationByHandle,globl
	.previous
