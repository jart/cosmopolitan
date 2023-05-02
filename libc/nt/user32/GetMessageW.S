#include "libc/nt/codegen.h"
.imp	user32,__imp_GetMessageW,GetMessageW,1899

#ifdef __x86_64__
	.text.windows
GetMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetMessageW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetMessage,globl
	.previous
#endif
