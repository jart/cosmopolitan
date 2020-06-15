.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_FindWindowExW,FindWindowExW,1783

	.text.windows
FindWindowEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindowEx,globl
	.previous
