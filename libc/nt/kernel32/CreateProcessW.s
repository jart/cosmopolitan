.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateProcessW,CreateProcessW,0

	.text.windows
CreateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	CreateProcess,globl
	.previous
