.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_EnumChildWindows,EnumChildWindows,1755

	.text.windows
EnumChildWindows:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EnumChildWindows(%rip),%rax
	jmp	__sysv2nt
	.endfn	EnumChildWindows,globl
	.previous
