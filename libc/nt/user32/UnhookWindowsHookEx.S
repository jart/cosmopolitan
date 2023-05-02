#include "libc/nt/codegen.h"
.imp	user32,__imp_UnhookWindowsHookEx,UnhookWindowsHookEx,2453

#ifdef __x86_64__
	.text.windows
UnhookWindowsHookEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_UnhookWindowsHookEx(%rip)
	leave
	ret
	.endfn	UnhookWindowsHookEx,globl
	.previous
#endif
