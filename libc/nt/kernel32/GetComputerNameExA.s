.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetComputerNameExA,GetComputerNameExA,0

	.text.windows
GetComputerNameExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetComputerNameExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetComputerNameExA,globl
	.previous
