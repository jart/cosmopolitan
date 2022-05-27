.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_Process32FirstW,Process32FirstW,0

	.text.windows
Process32First:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_Process32FirstW(%rip),%rax
	jmp	__sysv2nt
	.endfn	Process32First,globl
	.previous
