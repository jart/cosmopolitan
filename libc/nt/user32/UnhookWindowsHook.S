#include "libc/nt/codegen.h"
.imp	user32,__imp_UnhookWindowsHook,UnhookWindowsHook,2452

#ifdef __x86_64__
	.text.windows
UnhookWindowsHook:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnhookWindowsHook(%rip),%rax
	jmp	__sysv2nt
	.endfn	UnhookWindowsHook,globl
	.previous
#endif
