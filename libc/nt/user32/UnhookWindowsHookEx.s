.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_UnhookWindowsHookEx,UnhookWindowsHookEx,2453

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
