#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowTextW,SetWindowTextW,2405

#ifdef __x86_64__
	.text.windows
SetWindowText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowTextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowText,globl
	.previous
#endif
