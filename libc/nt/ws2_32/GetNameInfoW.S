#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetNameInfoW,GetNameInfoW,34

#ifdef __x86_64__
	.text.windows
GetNameInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetNameInfoW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	GetNameInfo,globl
	.previous
#endif
