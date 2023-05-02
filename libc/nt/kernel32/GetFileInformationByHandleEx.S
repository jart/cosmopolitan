#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileInformationByHandleEx,GetFileInformationByHandleEx,0

#ifdef __x86_64__
	.text.windows
GetFileInformationByHandleEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileInformationByHandleEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileInformationByHandleEx,globl
	.previous
#endif
