.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_FindWindowW,FindWindowW,1784

	.text.windows
FindWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindow,globl
	.previous
