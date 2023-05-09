#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowsHookExW,SetWindowsHookExW,2409

	.text.windows
SetWindowsHookEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetWindowsHookEx,globl
	.previous
