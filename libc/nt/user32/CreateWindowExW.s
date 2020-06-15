.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CreateWindowExW,CreateWindowExW,1625

	.text.windows
CreateWindowEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWindowExW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateWindowEx,globl
	.previous
