#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowsHookW,SetWindowsHookW,2410

	.text.windows
SetWindowsHook:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetWindowsHook,globl
	.previous
