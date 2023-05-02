#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowsHookExW,SetWindowsHookExW,2409

#ifdef __x86_64__
	.text.windows
SetWindowsHookEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHookEx,globl
	.previous
#endif
