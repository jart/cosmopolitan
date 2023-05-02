#include "libc/nt/codegen.h"
.imp	user32,__imp_SendMessageW,SendMessageW,2306

#ifdef __x86_64__
	.text.windows
SendMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SendMessageW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SendMessage,globl
	.previous
#endif
