#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowsHookW,SetWindowsHookW,2410

#ifdef __x86_64__
	.text.windows
SetWindowsHook:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHook,globl
	.previous
#endif
