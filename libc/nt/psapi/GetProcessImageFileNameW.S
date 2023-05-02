#include "libc/nt/codegen.h"
.imp	psapi,__imp_GetProcessImageFileNameW,GetProcessImageFileNameW,0

#ifdef __x86_64__
	.text.windows
GetProcessImageFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessImageFileNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessImageFileName,globl
	.previous
#endif
